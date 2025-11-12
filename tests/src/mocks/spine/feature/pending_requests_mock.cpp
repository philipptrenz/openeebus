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
 * @brief Pending Requests mock implementation
 */

#include "pending_requests_mock.h"

#include <gmock/gmock.h>

#include "src/common/eebus_malloc.h"
#include "src/spine/api/pending_requests_interface.h"

static void Destruct(PendingRequestsObject* self);
static void AddRequest(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, uint32_t max_delay);
static EebusError SetData(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, const FunctionData* data);
static EebusError SetResult(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, EebusError result);
static const FunctionData* GetData(const PendingRequestsObject* self, const char* ski, uint64_t msg_cnt);
static EebusError Remove(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt);

static const PendingRequestsInterface pending_requests_methods = {
    .destruct    = Destruct,
    .add_request = AddRequest,
    .set_data    = SetData,
    .set_result  = SetResult,
    .get_data    = GetData,
    .remove      = Remove,
};

static void PendingRequestsMockConstruct(PendingRequestsMock* self);

void PendingRequestsMockConstruct(PendingRequestsMock* self) {
  // Override "virtual functions table"
  PENDING_REQUESTS_INTERFACE(self) = &pending_requests_methods;
}

PendingRequestsMock* PendingRequestsMockCreate(void) {
  PendingRequestsMock* const mock = (PendingRequestsMock*)EEBUS_MALLOC(sizeof(PendingRequestsMock));

  PendingRequestsMockConstruct(mock);

  mock->gmock = new PendingRequestsGMock();

  return mock;
}

void Destruct(PendingRequestsObject* self) {
  PendingRequestsMock* const mock = PENDING_REQUESTS_MOCK(self);
  mock->gmock->Destruct(self);
  delete mock->gmock;
}

void AddRequest(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, uint32_t max_delay) {
  PendingRequestsMock* const mock = PENDING_REQUESTS_MOCK(self);
  mock->gmock->AddRequest(self, ski, msg_cnt, max_delay);
}

EebusError SetData(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, const FunctionData* data) {
  PendingRequestsMock* const mock = PENDING_REQUESTS_MOCK(self);
  return mock->gmock->SetData(self, ski, msg_cnt, data);
}

EebusError SetResult(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt, EebusError result) {
  PendingRequestsMock* const mock = PENDING_REQUESTS_MOCK(self);
  return mock->gmock->SetResult(self, ski, msg_cnt, result);
}

const FunctionData* GetData(const PendingRequestsObject* self, const char* ski, uint64_t msg_cnt) {
  PendingRequestsMock* const mock = PENDING_REQUESTS_MOCK(self);
  return mock->gmock->GetData(self, ski, msg_cnt);
}

EebusError Remove(PendingRequestsObject* self, const char* ski, uint64_t msg_cnt) {
  PendingRequestsMock* const mock = PENDING_REQUESTS_MOCK(self);
  return mock->gmock->Remove(self, ski, msg_cnt);
}
