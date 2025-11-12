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
 * @brief Eg Lpc Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_ACTOR_EG_LPC_EG_LPC_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_ACTOR_EG_LPC_EG_LPC_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/spine/model/entity_types.h"
#include "src/use_case/api/eg_lpc_listener_interface.h"

class EgLpcListenerGMockInterface {
 public:
  virtual ~EgLpcListenerGMockInterface() {};
  virtual void Destruct(EgLpcListenerObject* self)                                                       = 0;
  virtual void OnRemoteEntityConnect(EgLpcListenerObject* self, const EntityAddressType* entity_addr)    = 0;
  virtual void OnRemoteEntityDisconnect(EgLpcListenerObject* self, const EntityAddressType* entity_addr) = 0;
  virtual void OnPowerLimitReceive(
      EgLpcListenerObject* self,
      const ScaledValue* power_limit,
      const DurationType* duration,
      bool is_active
  )                                                                                                   = 0;
  virtual void OnFailsafePowerLimitReceive(EgLpcListenerObject* self, const ScaledValue* power_limit) = 0;
  virtual void OnFailsafeDurationReceive(EgLpcListenerObject* self, const DurationType* duration)     = 0;
  virtual void OnHeartbeatReceive(EgLpcListenerObject* self, uint64_t heartbeat_counter)              = 0;
};

class EgLpcListenerGMock : public EgLpcListenerGMockInterface {
 public:
  virtual ~EgLpcListenerGMock() {};
  MOCK_METHOD1(Destruct, void(EgLpcListenerObject*));
  MOCK_METHOD2(OnRemoteEntityConnect, void(EgLpcListenerObject*, const EntityAddressType*));
  MOCK_METHOD2(OnRemoteEntityDisconnect, void(EgLpcListenerObject*, const EntityAddressType*));
  MOCK_METHOD4(OnPowerLimitReceive, void(EgLpcListenerObject*, const ScaledValue*, const DurationType*, bool));
  MOCK_METHOD2(OnFailsafePowerLimitReceive, void(EgLpcListenerObject*, const ScaledValue*));
  MOCK_METHOD2(OnFailsafeDurationReceive, void(EgLpcListenerObject*, const DurationType*));
  MOCK_METHOD2(OnHeartbeatReceive, void(EgLpcListenerObject*, uint64_t));
};

typedef struct EgLpcListenerMock {
  /** Implements the Eg Lpc Listener Interface */
  EgLpcListenerObject obj;
  EgLpcListenerGMock* gmock;
} EgLpcListenerMock;

#define EG_LPC_LISTENER_MOCK(obj) ((EgLpcListenerMock*)(obj))

EgLpcListenerMock* EgLpcListenerMockCreate(void);

static inline void EgLpcListenerMockDelete(EgLpcListenerMock* self) {
  if (self != nullptr) {
    EG_LPC_LISTENER_DESTRUCT(EG_LPC_LISTENER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_ACTOR_EG_LPC_EG_LPC_LISTENER_MOCK_H_
