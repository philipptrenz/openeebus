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
 * @brief Cs Lpc Listener Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_USE_CASE_API_CS_LPC_LISTENER_MOCK_H_
#define TESTS_SRC_MOCKS_USE_CASE_API_CS_LPC_LISTENER_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/common/eebus_malloc.h"
#include "src/use_case/api/cs_lpc_listener_interface.h"

class CsLpcListenerGMockInterface {
 public:
  virtual ~CsLpcListenerGMockInterface() {};
  virtual void Destruct(CsLpcListenerObject* self) = 0;
  virtual void OnPowerLimitReceive(
      CsLpcListenerObject* self,
      const ScaledValue* power_limit,
      const DurationType* duration,
      bool is_active
  )                                                                                                   = 0;
  virtual void OnFailsafePowerLimitReceive(CsLpcListenerObject* self, const ScaledValue* power_limit) = 0;
  virtual void OnFailsafeDurationReceive(CsLpcListenerObject* self, const DurationType* duration)     = 0;
  virtual void OnHeartbeatReceive(CsLpcListenerObject* self, uint64_t heartbeat_counter)              = 0;
};

class CsLpcListenerGMock : public CsLpcListenerGMockInterface {
 public:
  virtual ~CsLpcListenerGMock() {};
  MOCK_METHOD1(Destruct, void(CsLpcListenerObject*));
  MOCK_METHOD4(OnPowerLimitReceive, void(CsLpcListenerObject*, const ScaledValue*, const DurationType*, bool));
  MOCK_METHOD2(OnFailsafePowerLimitReceive, void(CsLpcListenerObject*, const ScaledValue*));
  MOCK_METHOD2(OnFailsafeDurationReceive, void(CsLpcListenerObject*, const DurationType*));
  MOCK_METHOD2(OnHeartbeatReceive, void(CsLpcListenerObject*, uint64_t));
};

typedef struct CsLpcListenerMock {
  /** Implements the Cs Lpc Listener Interface */
  CsLpcListenerObject obj;
  CsLpcListenerGMock* gmock;
} CsLpcListenerMock;

#define CS_LPC_LISTENER_MOCK(obj) ((CsLpcListenerMock*)(obj))

CsLpcListenerMock* CsLpcListenerMockCreate(void);

static inline void CsLpcListenerMockDelete(CsLpcListenerMock* self) {
  if (self != nullptr) {
    CS_LPC_LISTENER_DESTRUCT(CS_LPC_LISTENER_OBJECT(self));
    EEBUS_FREE(self);
  }
}

#endif  // TESTS_SRC_MOCKS_USE_CASE_API_CS_LPC_LISTENER_MOCK_H_
