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
 * @file
 * @brief CS LPC Listener implementation
 */

#include <math.h>
#include <stdio.h>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cs_lpc_listener_interface.h"

typedef struct CsLpcListener CsLpcListener;

struct CsLpcListener {
  /** Implements the CS LPC Listener Interface */
  CsLpcListenerObject obj;
};

#define CS_LPC_LISTENER(obj) ((CsLpcListener*)(obj))

static void Destruct(CsLpcListenerObject* self);
static void OnPowerLimitReceive(
    CsLpcListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafePowerLimitReceive(CsLpcListenerObject* self, const ScaledValue* power_limit);
static void OnFailsafeDurationReceive(CsLpcListenerObject* self, const DurationType* duration);
static void OnHeartbeatReceive(CsLpcListenerObject* self, uint64_t heartbeat_counter);

static const CsLpcListenerInterface cs_lpc_listener_methods = {
    .destruct                        = Destruct,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
};

static void CsLpcListenerConstruct(CsLpcListener* self);

void CsLpcListenerConstruct(CsLpcListener* self) {
  // Override "virtual functions table"
  CS_LPC_LISTENER_INTERFACE(self) = &cs_lpc_listener_methods;
}

CsLpcListenerObject* CsLpcListenerCreate(void) {
  CsLpcListener* const cs_lpc_listener = (CsLpcListener*)EEBUS_MALLOC(sizeof(CsLpcListener));

  CsLpcListenerConstruct(cs_lpc_listener);

  return CS_LPC_LISTENER_OBJECT(cs_lpc_listener);
}

void Destruct(CsLpcListenerObject* self) {
  // Nothing to be deallocated yet
}

double GetValue(const ScaledValue* value) {
  return (double)(value->value) * pow(10, (double)value->scale);
}

void OnPowerLimitReceive(
    CsLpcListenerObject* self,
    const ScaledValue* power_limit,
    const EebusDuration* duration,
    bool is_active
) {
  CsLpcListener* const lpc_listener = CS_LPC_LISTENER(self);

  const double limit     = GetValue(power_limit);
  const uint64_t seconds = EebusDurationToSeconds(duration);
  printf(
      "New Limit received %4.0fW, duration = %llu seconds, active = %s\n",
      limit,
      (unsigned long long)seconds,
      is_active ? "true" : "false"
  );
}

void OnFailsafePowerLimitReceive(CsLpcListenerObject* self, const ScaledValue* power_limit) {
  CsLpcListener* const lpc_listener = CS_LPC_LISTENER(self);

  const double limit = GetValue(power_limit);
  printf("New Failsafe Consumption Active Power Limit received:  %4.0fW\n", limit);
}

void OnFailsafeDurationReceive(CsLpcListenerObject* self, const DurationType* duration) {
  CsLpcListener* const lpc_listener = CS_LPC_LISTENER(self);

  const uint64_t seconds = EebusDurationToSeconds(duration);
  printf("New Failsafe Duration Minimum received: %llu seconds\n", (unsigned long long)seconds);
}

void OnHeartbeatReceive(CsLpcListenerObject* self, uint64_t heartbeat_counter) {
  CsLpcListener* const lpc_listener = CS_LPC_LISTENER(self);
  printf("Heartbeat received, counter = %llu\n", (unsigned long long)heartbeat_counter);
}
