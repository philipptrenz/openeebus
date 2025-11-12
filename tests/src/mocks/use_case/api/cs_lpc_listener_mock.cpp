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
 * @brief Cs Lpc Listener mock implementation
 */

#include "cs_lpc_listener_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cs_lpc_listener_interface.h"

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

static void CsLpcListenerMockConstruct(CsLpcListenerMock* self);

void CsLpcListenerMockConstruct(CsLpcListenerMock* self) {
  // Override "virtual functions table"
  CS_LPC_LISTENER_INTERFACE(self) = &cs_lpc_listener_methods;
}

CsLpcListenerMock* CsLpcListenerMockCreate(void) {
  CsLpcListenerMock* const mock = (CsLpcListenerMock*)EEBUS_MALLOC(sizeof(CsLpcListenerMock));

  CsLpcListenerMockConstruct(mock);

  mock->gmock = new CsLpcListenerGMock();

  return mock;
}

void Destruct(CsLpcListenerObject* self) {
  CsLpcListenerMock* const mock = CS_LPC_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnPowerLimitReceive(
    CsLpcListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
) {
  CsLpcListenerMock* const mock = CS_LPC_LISTENER_MOCK(self);
  mock->gmock->OnPowerLimitReceive(self, power_limit, duration, is_active);
}

void OnFailsafePowerLimitReceive(CsLpcListenerObject* self, const ScaledValue* power_limit) {
  CsLpcListenerMock* const mock = CS_LPC_LISTENER_MOCK(self);
  mock->gmock->OnFailsafePowerLimitReceive(self, power_limit);
}

void OnFailsafeDurationReceive(CsLpcListenerObject* self, const DurationType* duration) {
  CsLpcListenerMock* const mock = CS_LPC_LISTENER_MOCK(self);
  mock->gmock->OnFailsafeDurationReceive(self, duration);
}

void OnHeartbeatReceive(CsLpcListenerObject* self, uint64_t heartbeat_counter) {
  CsLpcListenerMock* const mock = CS_LPC_LISTENER_MOCK(self);
  mock->gmock->OnHeartbeatReceive(self, heartbeat_counter);
}
