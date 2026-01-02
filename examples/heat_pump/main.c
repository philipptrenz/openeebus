/*
 * Copyright 2025 NIBE AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/**
 * @file main.cpp
 * @brief Heat Pump example main function
 */

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hpsrv.h"
#include "src/common/eebus_thread/eebus_thread.h"
#include "src/ship/tls_certificate/tls_certificate.h"

static bool should_terminate = false;

static int32_t ClampI32(int32_t v, int32_t lo, int32_t hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static int32_t RandRangeI32(int32_t lo, int32_t hi) {
  // inclusive range
  if (hi <= lo) return lo;
  return lo + (rand() % (hi - lo + 1));
}

void PrintUsage() {
  printf("General Usage:\n");
  printf("heat_pump <server_port> <remote_ski> <certificate_file> <private_key_file>\n");
}

void GracefulTerminate(int signal) {
  should_terminate = ((signal == SIGTERM) || (signal == SIGINT));
}

/**
 * Publish MPC values:
 * - Scenario 1: total active power + per-phase power (W * 100)
 * - Scenario 4: voltage per-phase (V * 100)
 * - Scenario 5: frequency (Hz * 100)
 * - Scenario 3: current per-phase (A * 100, per comment in header)
 */
static void PublishMpc(HpsrvObject* hpsrv, int32_t total_w) {
  // ---- Power split across phases (sum == total_w) ----
  int32_t a_w = (total_w * 34) / 100;
  int32_t b_w = (total_w * 33) / 100;
  int32_t c_w = total_w - a_w - b_w;

  // Add a small imbalance (+/- up to 5% of total), keep sum constant.
  int32_t max_imbalance = total_w / 20; // 5%
  if (max_imbalance < 1) max_imbalance = 1;

  int32_t da = RandRangeI32(-max_imbalance, +max_imbalance);
  int32_t db = RandRangeI32(-max_imbalance, +max_imbalance);

  a_w = ClampI32(a_w + da, 0, total_w);
  b_w = ClampI32(b_w + db, 0, total_w - a_w);
  c_w = total_w - a_w - b_w;

  // Scale power to W * 100
  const int32_t total_p = total_w * 100;
  const int32_t a_p     = a_w * 100;
  const int32_t b_p     = b_w * 100;
  const int32_t c_p     = c_w * 100;

  (void)HpsrvSetPowerTotal(hpsrv, total_p);
  (void)HpsrvSetPowerPerPhase(hpsrv, a_p, b_p, c_p);

  // ---- Voltage + Frequency (mostly stable, small noise) ----
  // L-N voltages around 230V, add +-2.00V noise
  int32_t v_an = 23000 + RandRangeI32(-200, +200);
  int32_t v_bn = 23000 + RandRangeI32(-200, +200);
  int32_t v_cn = 23000 + RandRangeI32(-200, +200);

  // L-L voltages around 400V, add +-3.00V noise
  int32_t v_ab = 40000 + RandRangeI32(-300, +300);
  int32_t v_bc = 40000 + RandRangeI32(-300, +300);
  int32_t v_ca = 40000 + RandRangeI32(-300, +300);

  (void)HpsrvSetVoltagePerPhase(hpsrv, v_an, v_bn, v_cn, v_ab, v_bc, v_ca);

  // Frequency around 50Hz, add +-0.05Hz noise
  int32_t f_hz = 5000 + RandRangeI32(-5, +5);
  (void)HpsrvSetAcFrequency(hpsrv, f_hz);

  // ---- Current derived from P and U: I ≈ P / U ----
  // We compute current in centi-amps: I_cA = (P_W * 100) / U_V
  // U in centi-volts -> U_V = U_cV / 100
  // => I_cA = (P_W * 100) / (U_cV / 100) = (P_W * 10000) / U_cV
  //
  // Use per-phase L-N voltages.
  int32_t i_a_cA = 0, i_b_cA = 0, i_c_cA = 0;

  if (v_an > 0) i_a_cA = (a_w * 10000) / v_an;
  if (v_bn > 0) i_b_cA = (b_w * 10000) / v_bn;
  if (v_cn > 0) i_c_cA = (c_w * 10000) / v_cn;

  // Add a tiny noise of +-0.10A (i.e. +-10 cA), clamp >= 0
  i_a_cA = ClampI32(i_a_cA + RandRangeI32(-10, +10), 0, 100000);
  i_b_cA = ClampI32(i_b_cA + RandRangeI32(-10, +10), 0, 100000);
  i_c_cA = ClampI32(i_c_cA + RandRangeI32(-10, +10), 0, 100000);

  (void)HpsrvSetAcCurrentPerPhase(hpsrv, i_a_cA, i_b_cA, i_c_cA);

  // Console log (human-readable)
  printf("[MPC] P: total=%4dW A=%4dW B=%4dW C=%4dW | "
         "U: AN=%3d.%02dV BN=%3d.%02dV CN=%3d.%02dV | "
         "I: A=%2d.%02dA B=%2d.%02dA C=%2d.%02dA | "
         "f=%2d.%02dHz\n",
         total_w, a_w, b_w, c_w,
         v_an / 100, abs(v_an % 100),
         v_bn / 100, abs(v_bn % 100),
         v_cn / 100, abs(v_cn % 100),
         i_a_cA / 100, abs(i_a_cA % 100),
         i_b_cA / 100, abs(i_b_cA % 100),
         i_c_cA / 100, abs(i_c_cA % 100),
         f_hz / 100, abs(f_hz % 100));
}

void MainLoop(HpsrvObject* hpsrv) {
  signal(SIGINT, &GracefulTerminate);
  signal(SIGTERM, &GracefulTerminate);

  srand((unsigned)time(NULL));

  // Start around 1.5 kW
  int32_t power_w = 1500;

  while (!should_terminate) {
    // Random walk: [-250..+250] W per second
    const int32_t delta = (rand() % 501) - 250;
    power_w             = ClampI32(power_w + delta, 0, 5000);

    PublishMpc(hpsrv, power_w);

    EebusThreadSleep(1);
  }
}

int main(int argc, char** argv) {
  if (argc != 5) {
    PrintUsage();
    return -1;
  }

  const int32_t port = atoi(argv[1]);

  const char* const remote_ski = argv[2];
  const char* const cert       = argv[3];
  const char* const pkey       = argv[4];

  TlsCertificateObject* const tls_cert = TlsCertificateLoadX509KeyPair(cert, pkey);
  if (tls_cert == NULL) {
    printf("Failed to load TLS certificate and private key!\n");
    return -1;
  }

  HpsrvObject* const hpsrv = HpsrvOpen(port, remote_ski, tls_cert);
  if (hpsrv == NULL) {
    printf("Failed to open heat pump EEBUS service!\n");
    return -1;
  }

  HpsrvRegisterRemoteSki(hpsrv, remote_ski);

  MainLoop(hpsrv);

  HpsrvClose(hpsrv);
  return 0;
}
