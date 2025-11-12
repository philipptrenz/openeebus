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

#include "src/use_case/actor/cs/lpc/cs_lpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/cs_lpc_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/actor/cs/lpc/device_configuration_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpc/device_configuration_description_request.inc"
#include "tests/src/use_case/actor/cs/lpc/device_configuration_key_value_list_request.inc"
#include "tests/src/use_case/actor/cs/lpc/device_configuration_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/device_diagnosis_heartbeat_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/device_diagnosis_heartbeat_request.inc"
#include "tests/src/use_case/actor/cs/lpc/device_diagnosis_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/discovery_request.inc"
#include "tests/src/use_case/actor/cs/lpc/discovery_response.inc"
#include "tests/src/use_case/actor/cs/lpc/electrical_connection_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/failsafe_duration_write.inc"
#include "tests/src/use_case/actor/cs/lpc/failsafe_power_limit_write.inc"
#include "tests/src/use_case/actor/cs/lpc/heartbeat_notify.inc"
#include "tests/src/use_case/actor/cs/lpc/limits_request.inc"
#include "tests/src/use_case/actor/cs/lpc/limits_write.inc"
#include "tests/src/use_case/actor/cs/lpc/load_control_binding_request.inc"
#include "tests/src/use_case/actor/cs/lpc/load_control_description_request.inc"
#include "tests/src/use_case/actor/cs/lpc/load_control_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/cs/lpc/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/cs/lpc/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/cs/lpc/use_case_reply.inc"
#include "tests/src/use_case/actor/cs/lpc/use_case_request.inc"

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

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

void PrintMessage(const uint8_t* msg, size_t msg_size) {
#if 0
  std::string_view s(reinterpret_cast<const char*>(msg));
  std::cout << "\n" << s << "\n" << std::endl;
#endif
}

void CsLpclTestInternal() {
  const EebusDeviceInfo device_info = {
      .type       = "EnergyManagementSystem",
      .vendor     = "Demo",
      .brand      = "Demo",
      .model      = "HEMS",
      .serial_num = "123456789",
      .ship_id    = "Demo",
      .address    = "d:_n:Demo_HEMS-123456789",
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

  std::unique_ptr<CsLpcListenerMock, decltype(&CsLpcListenerMockDelete)> cs_lpc_listener_mock{
      CsLpcListenerMockCreate(),
      CsLpcListenerMockDelete
  };

  std::unique_ptr<CsLpcUseCaseObject, decltype(&CsLpcUseCaseDelete)> use_case{
      CsLpcUseCaseCreate(entity, 0, CS_LPC_LISTENER_OBJECT(cs_lpc_listener_mock.get())),
      CsLpcUseCaseDelete
  };

  SetConsumptionLimit(use_case.get(), 4200, 0, false, true);

  DEVICE_LOCAL_ADD_ENTITY(device_local.get(), entity);

  // 1. Setup the Data Reader and expecte send the detailed discovery request
  EXPECT_CALL(*data_write_mock->gmock, WriteMessage(_, _, _)).WillRepeatedly(WithArgs<1, 2>(Invoke(PrintMessage)));
  DataReaderObject* const data_reader
      = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(device_local.get(), remote_ski, DATA_WRITER_OBJECT(data_write_mock.get()));
  // 2. Receive the detailed discovery request and send the repsonse
  HandleMessage(device_local.get(), data_reader, discovery_request, sizeof(discovery_request));
  // 3. Receive the detailed discovery and send the response
  HandleMessage(device_local.get(), data_reader, discovery_response, sizeof(discovery_response));
  // 4. Receive the Node Management dubscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      node_management_subscription_request,
      sizeof(node_management_subscription_request)
  );
  // 5. Receive the use case discovery and send the response
  HandleMessage(device_local.get(), data_reader, use_case_request, sizeof(use_case_request));
  // 6. Receive the load control subscription request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      load_control_subscription_request,
      sizeof(load_control_subscription_request)
  );
  // 7. Receive the load control binding request and send the response
  HandleMessage(device_local.get(), data_reader, load_control_binding_request, sizeof(load_control_binding_request));
  // 8. Receive the load control description read request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      load_control_description_request,
      sizeof(load_control_description_request)
  );
  // 9. Receive the device configuration subscription request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      device_configuration_subscription_request,
      sizeof(device_configuration_subscription_request)
  );
  // 10. Receive the device configuration binding request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      device_configuration_binding_request,
      sizeof(device_configuration_binding_request)
  );
  // 11. Receive the device configuration description request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      device_configuration_description_request,
      sizeof(device_configuration_description_request)
  );
  // 12. Receive the Device Diagnosis subscription request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      device_diagnosis_subscription_request,
      sizeof(device_diagnosis_subscription_request)
  );
  // 13. Receive the Electrical Connection subscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      electrical_connection_subscription_request,
      sizeof(electrical_connection_subscription_request)
  );
  // 14. Receive the Heartbeat subscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      device_diagnosis_heartbeat_request,
      sizeof(device_diagnosis_heartbeat_request)
  );

  // 15. Receive the result with message counter reference 3
  HandleMessage(device_local.get(), data_reader, result_data_msg_cnt_ref_3, sizeof(result_data_msg_cnt_ref_3));
  // 16. Receive the Use Case reply
  HandleMessage(device_local.get(), data_reader, use_case_reply, sizeof(use_case_reply));
  // 17. Receive the result with message counter reference 5
  HandleMessage(device_local.get(), data_reader, result_data_msg_cnt_ref_5, sizeof(result_data_msg_cnt_ref_5));
  // 18. Receive the Heartbeat reply
  HandleMessage(
      device_local.get(),
      data_reader,
      device_diagnosis_heartbeat_reply,
      sizeof(device_diagnosis_heartbeat_reply)
  );
  // 19. Receive the Limits request
  HandleMessage(device_local.get(), data_reader, limits_request, sizeof(limits_request));
  // 20. Receive the Device Cnfiguration  Key Value List request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      device_configuration_key_value_list_request,
      sizeof(device_configuration_key_value_list_request)
  );
  // 21. Receive the Load Control Limits write and process the new data
  EXPECT_CALL(*cs_lpc_listener_mock->gmock, OnPowerLimitReceive(_, _, _, _))
      .WillOnce(
          WithArgs<1, 2, 3>(Invoke([](const ScaledValue* power_limit, const DurationType* duration, bool is_active) {
            ASSERT_NE(power_limit, nullptr);
            ASSERT_NE(duration, nullptr);
            EXPECT_EQ(power_limit->value, 100);
            EXPECT_EQ(power_limit->scale, 0);
            EXPECT_EQ(duration->hours, 1);
            EXPECT_EQ(duration->minutes, 2);
            EXPECT_EQ(duration->seconds, 3);
            EXPECT_EQ(is_active, true);
          }))
      );
  HandleMessage(device_local.get(), data_reader, limits_write, sizeof(limits_write));

  // 22. Recieve the Failsafe Consumption Active Power Limit write and process the new data
  EXPECT_CALL(*cs_lpc_listener_mock->gmock, OnFailsafePowerLimitReceive(_, _))
      .WillOnce(WithArgs<1>(Invoke([](const ScaledValue* power_limit) {
        ASSERT_NE(power_limit, nullptr);
        EXPECT_EQ(power_limit->value, 14);
        EXPECT_EQ(power_limit->scale, 1);
      })));
  HandleMessage(device_local.get(), data_reader, failsafe_power_limit_write, sizeof(failsafe_power_limit_write));

  // 23. Recieve the Failsafe Duration Minimum write and process the new data
  EXPECT_CALL(*cs_lpc_listener_mock->gmock, OnFailsafeDurationReceive(_, _))
      .WillOnce(WithArgs<1>(Invoke([](const DurationType* duration) {
        ASSERT_NE(duration, nullptr);
        EXPECT_EQ(duration->hours, 1);
        EXPECT_EQ(duration->minutes, 2);
        EXPECT_EQ(duration->seconds, 5);
      })));
  HandleMessage(device_local.get(), data_reader, failsafe_duration_write, sizeof(failsafe_duration_write));

  // 24. Set the Consumption Nominal Maximum value
  const ScaledValue consumption_nominal_max_set{700, 1};
  SetConsumptionNominalMax(use_case.get(), &consumption_nominal_max_set);

  ScaledValue consumption_nominal_max_get = {0};
  // 25. Get the Consumption Nominal Maximum value
  const EebusError err = GetConsumptionNominalMax(use_case.get(), &consumption_nominal_max_get);
  EXPECT_EQ(err, kEebusErrorOk);
  EXPECT_EQ(consumption_nominal_max_get.value, 700);
  EXPECT_EQ(consumption_nominal_max_get.scale, 1);

  for (size_t i = 0; i < 40; ++i) {
    EXPECT_CALL(*cs_lpc_listener_mock->gmock, OnHeartbeatReceive(_, _)).WillOnce(Return());
    HandleMessage(device_local.get(), data_reader, heartbeat_notify, sizeof(heartbeat_notify));
  }

  EXPECT_CALL(*cs_lpc_listener_mock->gmock, Destruct(_)).WillOnce(Return());
  EXPECT_CALL(*data_write_mock->gmock, Destruct(_)).WillOnce(Return());
}

TEST(CsLpcTest, CsLpcTest) {
  CsLpclTestInternal();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
