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
 * @brief Currently it is not a regular unit test but more a "sand box"
 * to feed the SPINE Device with specific datagrams andcheck the outgoing messages printed.
 * @note Remember to enable the message printing in PrintMessage() before getting started
 */

#include "src/spine/device/device_local.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/eg_lpc_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "src/use_case/actor/eg/lpc/eg_lpc.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/actor/eg/lpc/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/eg/lpc/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/discovery_request.inc"
#include "tests/src/use_case/actor/eg/lpc/discovery_response.inc"
#include "tests/src/use_case/actor/eg/lpc/limits_description_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/limits_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/eg/lpc/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/eg/lpc/use_case_reply.inc"
#include "tests/src/use_case/actor/eg/lpc/use_case_request.inc"

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

void PrintMessage(const uint8_t* msg, size_t msg_size) {
#if 0
  std::string_view s(reinterpret_cast<const char*>(msg));
  std::cout << "\n" << s << "\n" << std::endl;
#endif
}

EebusTimerObject* EebusTimerCreate(EebusTimerTimeoutCallback cb, void* ctx) {
  return EEBUS_TIMER_OBJECT(EebusTimerMockCreate());
}

void HandleMessage(
    DeviceLocalObject* device_local,
    DataReaderObject* data_reader,
    const uint8_t* msg,
    size_t msg_size
) {
  MessageBuffer msg_buf;
  MessageBufferInitWithDeallocator(&msg_buf, const_cast<uint8_t*>(msg), msg_size, NULL);
  DATA_READER_HANDLE_MESSAGE(data_reader, &msg_buf);
  MessageBufferRelease(&msg_buf);
  HandleQueueMessage(device_local);
}

void EgLpcTestInternal() {
  const EebusDeviceInfo device_info = {
      .type       = "ChargingStation",
      .vendor     = "Demo",
      .brand      = "Demo",
      .model      = "EVSE",
      .serial_num = "123456789",
      .ship_id    = "Demo",
      .address    = "d:_n:Demo_ControlBox-123456789",
  };

  static constexpr NetworkManagementFeatureSetType feature_set = kNetworkManagementFeatureSetTypeSmart;

  static constexpr char remote_ski[] = "1111";

  std::unique_ptr<DataWriterMock, decltype(&DataWriterMockDelete)> data_write_mock{
      DataWriterMockCreate(),
      DataWriterMockDelete
  };
  std::unique_ptr<DeviceLocalObject, decltype(&DeviceLocalDelete)> device_local{
      DeviceLocalCreate(&device_info, &feature_set),
      DeviceLocalDelete
  };

  // Create the device entities and add it to the SPINE device
  static constexpr uint32_t heartbeat_timeout = 4;

  uint32_t entity_ids[1] = {static_cast<uint32_t>(VectorGetSize(DEVICE_LOCAL_GET_ENTITIES(device_local.get())))};

  EntityLocalObject* const entity = EntityLocalCreate(
      device_local.get(),
      kEntityTypeTypeGridGuard,
      entity_ids,
      ARRAY_SIZE(entity_ids),
      heartbeat_timeout
  );

  std::unique_ptr<EgLpcListenerMock, decltype(&EgLpcListenerMockDelete)> eg_lpc_listener_mock{
      EgLpcListenerMockCreate(),
      EgLpcListenerMockDelete
  };

  std::unique_ptr<EgLpcUseCaseObject, decltype(&EgLpcUseCaseDelete)> use_case{
      EgLpcUseCaseCreate(entity, EG_LPC_LISTENER_OBJECT(eg_lpc_listener_mock.get())),
      EgLpcUseCaseDelete
  };

  DEVICE_LOCAL_ADD_ENTITY(device_local.get(), entity);

  // 1. Setup the Data Reader and expecte send the detailed discovery request
  EXPECT_CALL(*data_write_mock->gmock, WriteMessage(_, _, _)).WillRepeatedly(WithArgs<1, 2>(Invoke(PrintMessage)));
  DataReaderObject* const data_reader
      = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(device_local.get(), remote_ski, DATA_WRITER_OBJECT(data_write_mock.get()));
  // 2. Receive the detailed discovery request and send the response
  HandleMessage(device_local.get(), data_reader, (uint8_t*)discovery_request, sizeof(discovery_request));
  // 3. Receive the detailed discovery and send the response
  EXPECT_CALL(*eg_lpc_listener_mock->gmock, OnRemoteEntityConnect(_, _)).WillOnce(Return());
  HandleMessage(device_local.get(), data_reader, (uint8_t*)discovery_response, sizeof(discovery_response));
  // 4. Receive the Node Management subscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      (uint8_t*)node_management_subscription_request,
      sizeof(node_management_subscription_request)
  );
  // 5. Received the usecase discovery and send the response
  HandleMessage(device_local.get(), data_reader, (uint8_t*)use_case_request, sizeof(use_case_request));
  // 6. Receive the Device Diagnosis subscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      (uint8_t*)device_diagnosis_subscription_request,
      sizeof(device_diagnosis_subscription_request)
  );
  // 7. Receive the Heartbeat subscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      (uint8_t*)device_diagnosis_heartbeat_request,
      sizeof(device_diagnosis_heartbeat_request)
  );
  // 8. Receive the result with message counter reference 3
  HandleMessage(
      device_local.get(),
      data_reader,
      (uint8_t*)result_data_msg_cnt_ref_3,
      sizeof(result_data_msg_cnt_ref_3)
  );
  // 9. Receive the Use Case reply
  HandleMessage(device_local.get(), data_reader, (uint8_t*)use_case_reply, sizeof(use_case_reply));
  // 10. Receive the Load Control Limit Description reply
  HandleMessage(device_local.get(), data_reader, (uint8_t*)limits_description_reply, sizeof(limits_description_reply));
  // 11. Receive the Load Control Limit reply
  EXPECT_CALL(*eg_lpc_listener_mock->gmock, OnPowerLimitReceive(_, _, _, _))
      .WillOnce(WithArgs<1, 2, 3>(Invoke([](const ScaledValue* value, const DurationType* duration, bool is_active) {
        ASSERT_NE(value, nullptr);
        EXPECT_EQ(value->value, 4200);
        EXPECT_EQ(value->scale, 0);
        EXPECT_EQ(is_active, false);
      })));
  HandleMessage(device_local.get(), data_reader, (uint8_t*)limits_reply, sizeof(limits_reply));

  EXPECT_CALL(*eg_lpc_listener_mock->gmock, Destruct(_)).WillOnce(Return());
  EXPECT_CALL(*data_write_mock->gmock, Destruct(_)).WillOnce(Return());
}

TEST(EgLpcTest, EgLpcTest) {
  EgLpcTestInternal();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
