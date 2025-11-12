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
 * @brief Eg Lpc Listener mock implementation
 */

#include "eg_lpc_listener_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/eg_lpc_listener_interface.h"

static void Destruct(EgLpcListenerObject* self);
static void OnRemoteEntityConnect(EgLpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteEntityDisconnect(EgLpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnPowerLimitReceive(
    EgLpcListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
);
static void OnFailsafePowerLimitReceive(EgLpcListenerObject* self, const ScaledValue* power_limit);
static void OnFailsafeDurationReceive(EgLpcListenerObject* self, const DurationType* duration);
static void OnHeartbeatReceive(EgLpcListenerObject* self, uint64_t heartbeat_counter);

static const EgLpcListenerInterface eg_lpc_listener_methods = {
    .destruct                        = Destruct,
    .on_remote_entity_connect        = OnRemoteEntityConnect,
    .on_remote_entity_disconnect     = OnRemoteEntityDisconnect,
    .on_power_limit_receive          = OnPowerLimitReceive,
    .on_failsafe_power_limit_receive = OnFailsafePowerLimitReceive,
    .on_failsafe_duration_receive    = OnFailsafeDurationReceive,
    .on_heartbeat_receive            = OnHeartbeatReceive,
};

static void EgLpcListenerMockConstruct(EgLpcListenerMock* self);

void EgLpcListenerMockConstruct(EgLpcListenerMock* self) {
  // Override "virtual functions table"
  EG_LPC_LISTENER_INTERFACE(self) = &eg_lpc_listener_methods;
}

EgLpcListenerMock* EgLpcListenerMockCreate(void) {
  EgLpcListenerMock* const mock = (EgLpcListenerMock*)EEBUS_MALLOC(sizeof(EgLpcListenerMock));

  EgLpcListenerMockConstruct(mock);

  mock->gmock = new EgLpcListenerGMock();

  return mock;
}

void Destruct(EgLpcListenerObject* self) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void OnRemoteEntityConnect(EgLpcListenerObject* self, const EntityAddressType* entity_addr) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->OnRemoteEntityConnect(self, entity_addr);
}

void OnRemoteEntityDisconnect(EgLpcListenerObject* self, const EntityAddressType* entity_addr) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->OnRemoteEntityDisconnect(self, entity_addr);
}

void OnPowerLimitReceive(
    EgLpcListenerObject* self,
    const ScaledValue* power_limit,
    const DurationType* duration,
    bool is_active
) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->OnPowerLimitReceive(self, power_limit, duration, is_active);
}

void OnFailsafePowerLimitReceive(EgLpcListenerObject* self, const ScaledValue* power_limit) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->OnFailsafePowerLimitReceive(self, power_limit);
}

void OnFailsafeDurationReceive(EgLpcListenerObject* self, const DurationType* duration) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->OnFailsafeDurationReceive(self, duration);
}

void OnHeartbeatReceive(EgLpcListenerObject* self, uint64_t heartbeat_counter) {
  EgLpcListenerMock* const mock = EG_LPC_LISTENER_MOCK(self);
  mock->gmock->OnHeartbeatReceive(self, heartbeat_counter);
}
