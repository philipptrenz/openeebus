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

#include "src/use_case/actor/mu/mpc/mu_mpc.h"

#include <gtest/gtest.h>

#include <memory>

#include "mocks/common/eebus_timer/eebus_timer_mock.h"
#include "mocks/ship/ship_connection/data_writer_mock.h"
#include "src/common/array_util.h"
#include "src/common/eebus_malloc.h"
#include "src/common/eebus_timer/eebus_timer.h"
#include "src/common/message_buffer.h"
#include "src/spine/device/device_local.h"
#include "src/spine/device/device_local_internal.h"
#include "src/spine/entity/entity_local.h"
#include "tests/src/json.h"
#include "tests/src/memory_leak.inc"
#include "tests/src/use_case/actor/mu/mpc/discovery_request.inc"
#include "tests/src/use_case/actor/mu/mpc/discovery_response.inc"
#include "tests/src/use_case/actor/mu/mpc/electrical_connection_parameter_description_request.inc"
#include "tests/src/use_case/actor/mu/mpc/electrical_connection_request.inc"
#include "tests/src/use_case/actor/mu/mpc/electrical_connection_subscription_request.inc"
#include "tests/src/use_case/actor/mu/mpc/measurement_constraints_request.inc"
#include "tests/src/use_case/actor/mu/mpc/measurement_description_request.inc"
#include "tests/src/use_case/actor/mu/mpc/measurement_subscription_request.inc"
#include "tests/src/use_case/actor/mu/mpc/node_management_subscription_request.inc"
#include "tests/src/use_case/actor/mu/mpc/result_data_msg_cnt_ref_3.inc"
#include "tests/src/use_case/actor/mu/mpc/use_case_reply.inc"
#include "tests/src/use_case/actor/mu/mpc/use_case_request.inc"

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

void MuMpcTestInternal() {
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
      kEntityTypeTypeHeatPumpAppliance,
      entity_ids,
      ARRAY_SIZE(entity_ids),
      heartbeat_timeout
  );

  static constexpr MuMpcMeasurementConfig measurement_default_cfg = {
      .value_source = kMeasurementValueSourceTypeMeasuredValue,
  };

  static constexpr MuMpcMonitorEnergyConfig energy_cfg{
      .energy_production_cfg  = &measurement_default_cfg,
      .energy_consumption_cfg = &measurement_default_cfg,
  };

  static constexpr MuMpcMonitorCurrentConfig current_cfg = {
      .current_phase_a_cfg = &measurement_default_cfg,
  };

  static constexpr MuMpcMonitorFrequencyConfig frequency_cfg = {
      .frequency_cfg = measurement_default_cfg,
  };

  static constexpr MuMpcConfig cfg {
    .power_cfg = {
        .power_total_cfg   = measurement_default_cfg,
        .power_phase_a_cfg = &measurement_default_cfg,
    },

    .energy_cfg    = &energy_cfg,
    .current_cfg   = &current_cfg,
    .frequency_cfg = &frequency_cfg
  };

  std::unique_ptr<MuMpcUseCaseObject, decltype(&MuMpcUseCaseDelete)> use_case{
      MuMpcUseCaseCreate(entity, 1, &cfg),
      MuMpcUseCaseDelete
  };

  static constexpr ScaledValue power_total = {1000, 0};
  MuMpcSetMeasurementDataCache(use_case.get(), kMpcPowerTotal, &power_total, NULL, NULL);

  static constexpr ScaledValue current_phase_a = {33, -1};
  static constexpr EebusDateTime timestamp     = {
          .date = {.year = 2025, .month = 7, .day = 1},
          .time = {  .hour = 12,   .min = 0, .sec = 0}
  };

  MuMpcSetMeasurementDataCache(use_case.get(), kMpcCurrentPhaseA, &current_phase_a, &timestamp, NULL);

  static constexpr ScaledValue energy_consumed = {5000, 0};
  static constexpr EebusDateTime start_time    = {
         .date = {.year = 2025, .month = 9, .day = 1},
         .time = {   .hour = 0,   .min = 0, .sec = 0}
  };

  static constexpr EebusDateTime end_time = {
      .date = {.year = 2025, .month = 10, .day = 2},
      .time = {   .hour = 0,    .min = 0, .sec = 0}
  };

  MuMpcSetEnergyConsumedCache(use_case.get(), &energy_consumed, NULL, NULL, &start_time, &end_time);

  static constexpr ScaledValue energy_produced = {2000, 0};
  MuMpcSetEnergyProducedCache(use_case.get(), &energy_produced, NULL, NULL, &start_time, &end_time);

  static constexpr ScaledValue frequency = {50, 0};
  MuMpcSetMeasurementDataCache(use_case.get(), kMpcFrequency, &frequency, NULL, NULL);

  MuMpcUpdate(use_case.get());

  ScaledValue value = {0};
  MuMpcGetMeasurementData(use_case.get(), kMpcPowerTotal, &value);
  EXPECT_EQ(value.value, 1000);
  EXPECT_EQ(value.scale, 0);

  MuMpcGetMeasurementData(use_case.get(), kMpcCurrentPhaseA, &value);
  EXPECT_EQ(value.value, 33);
  EXPECT_EQ(value.scale, -1);

  MuMpcGetMeasurementData(use_case.get(), kMpcFrequency, &value);
  EXPECT_EQ(value.value, 50);
  EXPECT_EQ(value.scale, 0);

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
  // 6. Receive the electrical conncetion subscription request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      electrical_connection_subscription_request,
      sizeof(electrical_connection_subscription_request)
  );
  // 7. Receive the electrical connection read request and send the response
  HandleMessage(device_local.get(), data_reader, electrical_connection_request, sizeof(electrical_connection_request));
  // 8. Receive the electrical connection parameter description request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      electrical_connection_parameter_description_request,
      sizeof(electrical_connection_parameter_description_request)
  );
  // 9. Receive the measurement subscription request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      measurement_subscription_request,
      sizeof(measurement_subscription_request)
  );
  // 10. Receive the measurement description request request
  HandleMessage(
      device_local.get(),
      data_reader,
      measurement_description_request,
      sizeof(measurement_description_request)
  );
  // 11. Receive the measurement constraints request request and send the response
  HandleMessage(
      device_local.get(),
      data_reader,
      measurement_constraints_request,
      sizeof(measurement_constraints_request)
  );
  // 12. Receive the result with message counter reference 3
  HandleMessage(device_local.get(), data_reader, result_data_msg_cnt_ref_3, sizeof(result_data_msg_cnt_ref_3));
  // 13. Receive the Use Case reply
  HandleMessage(device_local.get(), data_reader, use_case_reply, sizeof(use_case_reply));

  EXPECT_CALL(*data_write_mock->gmock, Destruct(_)).WillOnce(Return());
}

TEST(MuMpcTest, MuMpcTest) {
  MuMpcTestInternal();
  EXPECT_EQ(heap_used, 0);
  CheckForMemoryLeaks();
}
