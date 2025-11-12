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
 * to feed the SPINE Device with specific datagrams and check the outgoing messages printed.
 * @note Remember to enable the message printing in PrintMessage() before getting started
 */

#include "src/use_case/actor/ma/mpc/ma_mpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "mocks/use_case/api/ma_mpc_listener_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/actor/ma/mpc/discovery_request.inc"
#include "tests/src/use_case/actor/ma/mpc/discovery_response.inc"
#include "tests/src/use_case/actor/ma/mpc/electrical_connection_description_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/electrical_connection_parameter_description_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_constraints_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_description_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_notify_current.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_notify_energy.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_notify_frequency.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_notify_power.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_notify_voltage.inc"
#include "tests/src/use_case/actor/ma/mpc/measurement_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/ma/mpc/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/ma/mpc/result_data_msg_cnt_ref_5.inc"
#include "tests/src/use_case/actor/ma/mpc/result_data_msg_cnt_ref_8.inc"
#include "tests/src/use_case/actor/ma/mpc/use_case_reply.inc"
#include "tests/src/use_case/actor/ma/mpc/use_case_request.inc"

using testing::_;
using testing::Invoke;
using testing::Return;
using testing::WithArgs;

auto ScaledValueMatcher = [](int64_t value, int8_t scale) {
  return testing::AllOf(testing::Field(&ScaledValue::value, value), testing::Field(&ScaledValue::scale, scale));
};

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

void ExpectMeasurementsReceive(
    MaMpcListenerGMock* mock,
    const std::map<MuMpcMeasurementNameId, ScaledValue>& expected_measurements
) {
  for (const auto& [name_id, scaled_value] : expected_measurements) {
    const int64_t value = scaled_value.value;
    const int8_t scale  = scaled_value.scale;
    EXPECT_CALL(*mock, OnMeasurementReceive(_, name_id, ScaledValueMatcher(value, scale), _)).WillOnce(Return());
  }
}

void MaMpcTestInternal() {
  constexpr char device_addr[] = "d:_n:OpenEEBUS_123456789";

  const EebusDeviceInfo device_info = {
      .type       = "EnergyManagementSystem",
      .vendor     = "Demo",
      .brand      = "Demo",
      .model      = "HEMS",
      .serial_num = "123456789",
      .ship_id    = "Demo",
      .address    = device_addr,
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
      kEntityTypeTypeCEM,
      entity_ids,
      ARRAY_SIZE(entity_ids),
      heartbeat_timeout
  );

  std::unique_ptr<MaMpcListenerMock, decltype(&MaMpcListenerMockDelete)> ma_mpc_listener_mock{
      MaMpcListenerMockCreate(),
      MaMpcListenerMockDelete
  };

  std::unique_ptr<MaMpcUseCaseObject, decltype(&MaMpcUseCaseDelete)> use_case{
      MaMpcUseCaseCreate(entity, MA_MPC_LISTENER_OBJECT(ma_mpc_listener_mock.get())),
      MaMpcUseCaseDelete
  };

  DEVICE_LOCAL_ADD_ENTITY(device_local.get(), entity);

  // 1. Setup the Data Reader and expect to send the detailed discovery request
  EXPECT_CALL(*data_write_mock->gmock, WriteMessage(_, _, _)).WillRepeatedly(WithArgs<1, 2>(Invoke(PrintMessage)));
  DataReaderObject* const data_reader
      = DEVICE_LOCAL_SETUP_REMOTE_DEVICE(device_local.get(), remote_ski, DATA_WRITER_OBJECT(data_write_mock.get()));
  // 2. Receive the detailed discovery request and send the response
  HandleMessage(device_local.get(), data_reader, discovery_request, sizeof(discovery_request));
  // 3. Receive the detailed discovery and send the response
  EXPECT_CALL(*ma_mpc_listener_mock->gmock, OnRemoteEntityConnect(_, _)).WillOnce(Return());
  HandleMessage(device_local.get(), data_reader, discovery_response, sizeof(discovery_response));
  // 4. Receive the Node Management subscription request
  HandleMessage(
      device_local.get(),
      data_reader,
      node_management_subscription_request,
      sizeof(node_management_subscription_request)
  );
  // 5. Receive the use case discovery and send the response
  HandleMessage(device_local.get(), data_reader, use_case_request, sizeof(use_case_request));

  // 6. Receive the result with message counter reference 3
  HandleMessage(device_local.get(), data_reader, result_data_msg_cnt_ref_3, sizeof(result_data_msg_cnt_ref_3));

  // 7. Receive the Use Case reply
  HandleMessage(device_local.get(), data_reader, use_case_reply, sizeof(use_case_reply));

  // 8. Receive the result with message counter reference 5
  HandleMessage(device_local.get(), data_reader, result_data_msg_cnt_ref_5, sizeof(result_data_msg_cnt_ref_5));

  // 9. Receive the electrical connection description reply
  HandleMessage(
      device_local.get(),
      data_reader,
      electrical_connection_description_reply,
      sizeof(electrical_connection_description_reply)
  );

  // 10. Receive the electrical connection parameter description reply
  HandleMessage(
      device_local.get(),
      data_reader,
      electrical_connection_parameter_description_reply,
      sizeof(electrical_connection_parameter_description_reply)
  );

  // 11. Receive the result with message counter reference 8
  HandleMessage(device_local.get(), data_reader, result_data_msg_cnt_ref_8, sizeof(result_data_msg_cnt_ref_8));

  // 12. Receive the measurement description reply
  HandleMessage(device_local.get(), data_reader, measurement_description_reply, sizeof(measurement_description_reply));

  // 13. Receive the measurement constraints reply
  HandleMessage(device_local.get(), data_reader, measurement_constraints_reply, sizeof(measurement_constraints_reply));

  // 14. Receive the measurement reply
  constexpr ScaledValue power_total_expected = {.value = 33000, .scale = -1};
  EXPECT_CALL(*ma_mpc_listener_mock->gmock, OnMeasurementReceive(_, kMpcPowerTotal, ScaledValueMatcher(33000, -1), _))
      .WillOnce(Return());

  HandleMessage(device_local.get(), data_reader, measurement_reply, sizeof(measurement_reply));

  // 15. Receive the measurement notify (power)
  const std::map<MuMpcMeasurementNameId, ScaledValue> expected_power{
      {kMpcPowerPhaseA, {.value = 1000, .scale = 0}},
      {kMpcPowerPhaseB, {.value = 1100, .scale = 0}},
      {kMpcPowerPhaseC, {.value = 1200, .scale = 0}},
  };

  ExpectMeasurementsReceive(ma_mpc_listener_mock->gmock, expected_power);
  HandleMessage(device_local.get(), data_reader, measurement_notify_power, sizeof(measurement_notify_power));

  // 16. Receive the measurement notify (energy)
  const std::map<MuMpcMeasurementNameId, ScaledValue> expected_energy{
      {kMpcEnergyConsumed,  {.value = 550000, .scale = 0}},
      {kMpcEnergyProduced, {.value = 210007, .scale = -1}},
  };

  ExpectMeasurementsReceive(ma_mpc_listener_mock->gmock, expected_energy);
  HandleMessage(device_local.get(), data_reader, measurement_notify_energy, sizeof(measurement_notify_energy));

  // 17. Receive the measurement notify (current)
  const std::map<MuMpcMeasurementNameId, ScaledValue> expected_current{
      {kMpcCurrentPhaseA, {.value = 33, .scale = -2}},
      {kMpcCurrentPhaseB, {.value = 51, .scale = -2}},
      {kMpcCurrentPhaseC, {.value = 13, .scale = -3}},
  };

  ExpectMeasurementsReceive(ma_mpc_listener_mock->gmock, expected_current);
  HandleMessage(device_local.get(), data_reader, measurement_notify_current, sizeof(measurement_notify_current));

  // 18. Receive the measurement notify (voltage)
  const std::map<MuMpcMeasurementNameId, ScaledValue> expected_voltage{
      { kMpcVoltagePhaseA,    {.value = 110, .scale = 0}},
      { kMpcVoltagePhaseB,  {.value = 1205, .scale = -1}},
      { kMpcVoltagePhaseC,    {.value = 130, .scale = 0}},
      {kMpcVoltagePhaseAb, {.value = 14037, .scale = -2}},
      {kMpcVoltagePhaseBc,    {.value = 150, .scale = 0}},
      {kMpcVoltagePhaseAc,     {.value = 16, .scale = 1}},
  };

  ExpectMeasurementsReceive(ma_mpc_listener_mock->gmock, expected_voltage);
  HandleMessage(device_local.get(), data_reader, measurement_notify_voltage, sizeof(measurement_notify_voltage));

  // 19. Receive the measurement notify (frequency)
  const std::map<MuMpcMeasurementNameId, ScaledValue> expected_frequency{
      {kMpcFrequency, {.value = 500, .scale = -1}},
  };

  ExpectMeasurementsReceive(ma_mpc_listener_mock->gmock, expected_frequency);
  HandleMessage(device_local.get(), data_reader, measurement_notify_frequency, sizeof(measurement_notify_frequency));

  // 20. Get all of the measurements received via GetMeasurementData() and check the values
  ScaledValue value;
  static constexpr uint32_t remote_entity_id = 1;

  static constexpr const uint32_t* const remote_entity_ids[] = {&remote_entity_id};

  const EntityAddressType remote_entity_addr
      = {"d:_n:NIBE_06613320300999", remote_entity_ids, ARRAY_SIZE(remote_entity_ids)};

  std::map<MuMpcMeasurementNameId, ScaledValue> expected_data = expected_power;
  expected_data.insert(expected_energy.begin(), expected_energy.end());
  expected_data.insert(expected_current.begin(), expected_current.end());
  expected_data.insert(expected_voltage.begin(), expected_voltage.end());
  expected_data.insert(expected_frequency.begin(), expected_frequency.end());

  for (const auto& [name_id, scaled_value] : expected_data) {
    MaMpcGetMeasurementData(use_case.get(), name_id, &remote_entity_addr, &value);
    EXPECT_THAT(value, ScaledValueMatcher(scaled_value.value, scaled_value.scale));
  }

  EXPECT_CALL(*data_write_mock->gmock, Destruct(_)).WillOnce(Return());
  EXPECT_CALL(*ma_mpc_listener_mock->gmock, Destruct(_)).WillOnce(Return());
}

TEST(MaMpcTest, MaMpcTest) {
  MaMpcTestInternal();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
