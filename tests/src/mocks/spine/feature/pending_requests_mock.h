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
 * @brief Pending Requests Mock "class"
 */

#ifndef TESTS_SRC_MOCKS_SPINE_FEATURE_PENDING_REQUESTS_MOCK_H_
#define TESTS_SRC_MOCKS_SPINE_FEATURE_PENDING_REQUESTS_MOCK_H_

#include <gmock/gmock.h>

#include <memory>

#include "src/spine/api/pending_requests_interface.h"

class PendingRequestsGMockInterface {
 public:
  virtual ~PendingRequestsGMockInterface() {};
  virtual void Destruct(PendingRequestsObject* self)                                                          = 0;
  virtual void AddRequest(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, uint32_t max_delay) = 0;
  virtual EebusError SetData(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, const FunctionData* data)
      = 0;
  virtual EebusError SetResult(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, EebusError result) = 0;
  virtual const FunctionData* GetData(const PendingRequestsObject* self, const char* ski, uint64_t msg_cnt)       = 0;
  virtual EebusError Remove(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt)                       = 0;
};

class PendingRequestsGMock : public PendingRequestsGMockInterface {
 public:
  virtual ~PendingRequestsGMock() {};
  MOCK_METHOD1(Destruct, void(PendingRequestsObject*));
  MOCK_METHOD4(AddRequest, void(PendingRequestsObject*, const char*, uint64_t, uint32_t));
  MOCK_METHOD4(SetData, EebusError(PendingRequestsObject*, const char*, uint64_t, const FunctionData*));
  MOCK_METHOD4(SetResult, EebusError(PendingRequestsObject*, const char*, uint64_t, EebusError));
  MOCK_METHOD3(GetData, const FunctionData*(const PendingRequestsObject*, const char*, uint64_t));
  MOCK_METHOD3(Remove, EebusError(PendingRequestsObject*, const char*, uint64_t));
};

typedef struct PendingRequestsMock {
  /** Implements the Pending Requests Interface */
  PendingRequestsObject obj;
  PendingRequestsGMock* gmock;
} PendingRequestsMock;

#define PENDING_REQUESTS_MOCK(obj) ((PendingRequestsMock*)(obj))

PendingRequestsMock* PendingRequestsMockCreate(void);

#endif  // TESTS_SRC_MOCKS_SPINE_FEATURE_PENDING_REQUESTS_MOCK_H_
