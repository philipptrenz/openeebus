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
 * @brief Ma Mpc Listener implementation
 */

#include <math.h>
#include <stdio.h>

#include "examples/hems/hems.h"
#include "src/common/eebus_malloc.h"
#include "src/use_case/api/ma_mpc_listener_interface.h"

typedef struct MaMpcListener MaMpcListener;

struct MaMpcListener {
  /** Implements the Ma Mpc Listener Interface */
  MaMpcListenerObject obj;

  /* Pointer to the HEMS instance */
  HemsObject* hems;
};

#define MA_MPC_LISTENER(obj) ((MaMpcListener*)(obj))

static void Destruct(MaMpcListenerObject* self);
static void OnRemoteEntityConnect(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnRemoteEntityDisconnect(MaMpcListenerObject* self, const EntityAddressType* entity_addr);
static void OnMeasurementReceive(
    MaMpcListenerObject* self,
    MuMpcMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
);

static const MaMpcListenerInterface ma_mpc_listener_methods = {
    .destruct                    = Destruct,
    .on_remote_entity_connect    = OnRemoteEntityConnect,
    .on_remote_entity_disconnect = OnRemoteEntityDisconnect,
    .on_measurement_receive      = OnMeasurementReceive,
};

static void MaMpcListenerConstruct(MaMpcListener* self, HemsObject* hems);
static const char* MuMpcMeasurementNameIdToString(MuMpcMeasurementNameId name_id);
static float GetValue(const ScaledValue* value);

void MaMpcListenerConstruct(MaMpcListener* self, HemsObject* hems) {
  // Override "virtual functions table"
  MA_MPC_LISTENER_INTERFACE(self) = &ma_mpc_listener_methods;

  self->hems = hems;
}

MaMpcListenerObject* MaMpcListenerCreate(HemsObject* hems) {
  MaMpcListener* const ma_mpc_listener = (MaMpcListener*)EEBUS_MALLOC(sizeof(MaMpcListener));

  MaMpcListenerConstruct(ma_mpc_listener, hems);

  return MA_MPC_LISTENER_OBJECT(ma_mpc_listener);
}

void Destruct(MaMpcListenerObject* self) {
  // TODO: Implement destructor
}

void OnRemoteEntityConnect(MaMpcListenerObject* self, const EntityAddressType* entity_addr) {
  MaMpcListener* const ma_mpc_listener = MA_MPC_LISTENER(self);

  HemsSetMaMpcRemoteEntity(ma_mpc_listener->hems, entity_addr);
}

void OnRemoteEntityDisconnect(MaMpcListenerObject* self, const EntityAddressType* entity_addr) {
  MaMpcListener* const ma_mpc_listener = MA_MPC_LISTENER(self);

  HemsSetMaMpcRemoteEntity(ma_mpc_listener->hems, entity_addr);
}

const char* MuMpcMeasurementNameIdToString(MuMpcMeasurementNameId name_id) {
  switch (name_id) {
    case kMpcPowerTotal: return "Power Total";
    case kMpcPowerPhaseA: return "Power Phase A";
    case kMpcPowerPhaseB: return "Power Phase B";
    case kMpcPowerPhaseC: return "Power Phase C";
    case kMpcEnergyConsumed: return "Energy Consumed";
    case kMpcEnergyProduced: return "Energy Produced";
    case kMpcCurrentPhaseA: return "Current Phase A";
    case kMpcCurrentPhaseB: return "Current Phase B";
    case kMpcCurrentPhaseC: return "Current Phase C";
    case kMpcVoltagePhaseA: return "Voltage Phase A";
    case kMpcVoltagePhaseB: return "Voltage Phase B";
    case kMpcVoltagePhaseC: return "Voltage Phase C";
    case kMpcVoltagePhaseAb: return "Voltage Phase AB";
    case kMpcVoltagePhaseBc: return "Voltage Phase BC";
    case kMpcVoltagePhaseAc: return "Voltage Phase AC";
    case kMpcFrequency: return "Frequency";
    default: return "Unknown Measurement";
  }
}

float GetValue(const ScaledValue* value) {
  if (value == NULL) {
    return 0.0;
  }

  return value->value * pow(10, (float)value->scale);
}

void OnMeasurementReceive(
    MaMpcListenerObject* self,
    MuMpcMeasurementNameId name_id,
    const ScaledValue* measurement_value,
    const EntityAddressType* remote_entity_addr
) {
  const char* name  = MuMpcMeasurementNameIdToString(name_id);
  const float value = GetValue(measurement_value);

  printf("MA MPC Measurement received: %s = %.3f\n", name, value);
}
