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
 * @brief Feature Remote implementation
 */

#include "src/common/eebus_malloc.h"
#include "src/spine/api/entity_remote_interface.h"
#include "src/spine/api/feature_remote_interface.h"
#include "src/spine/api/function_interface.h"
#include "src/spine/feature/feature.h"

typedef struct FeatureRemote FeatureRemote;

struct FeatureRemote {
  /** Inherits the Feature */
  Feature obj;

  EntityRemoteObject* entity;
  uint32_t max_response_delay;
};

#define FEATURE_REMOTE(obj) ((FeatureRemote*)(obj))

static void Destruct(FeatureObject* self);
static DeviceRemoteObject* GetDevice(const FeatureRemoteObject* self);
static EntityRemoteObject* GetEntity(const FeatureRemoteObject* self);
static const void* GetData(const FeatureRemoteObject* self, FunctionType function_type);
static void* DataCopy(const FeatureRemoteObject* self, FunctionType fcn_type);
static EebusError UpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
);
static void SetFunctionOperations(
    FeatureRemoteObject* self,
    const FunctionPropertyType* const* supported_functions,
    size_t supported_functions_size
);
static void SetMaxResponseDelay(FeatureRemoteObject* self, uint32_t max_delay);
static uint32_t GetMaxResponseDelay(const FeatureRemoteObject* self);

static const FeatureRemoteInterface feature_remote_methods = {
    .feature_interface = {
        .destruct                = Destruct,
        .get_address             = FeatureGetAddress,
        .get_type                = FeatureGetType,
        .get_role                = FeatureGetRole,
        .get_function_operations = FeatureGetFunctionOperations,
        .get_description         = FeatureGetDescription,
        .set_description         = FeatureSetDescription,
        .to_string               = FeatureToString,
    },

    .get_device              = GetDevice,
    .get_entity              = GetEntity,
    .get_data                = GetData,
    .data_copy               = DataCopy,
    .update_data             = UpdateData,
    .set_function_operations = SetFunctionOperations,
    .set_max_response_delay  = SetMaxResponseDelay,
    .get_max_response_delay  = GetMaxResponseDelay,
};

static void FeatureRemoteConstruct(
    FeatureRemote* self,
    uint32_t id,
    EntityRemoteObject* entity,
    FeatureTypeType type,
    RoleType role
);
static void SetOperations(FeatureRemoteObject* self, FunctionType function_type, const PossibleOperationsType* ops);

void FeatureRemoteConstruct(
    FeatureRemote* self,
    uint32_t id,
    EntityRemoteObject* entity,
    FeatureTypeType type,
    RoleType role
) {
  FeatureConstruct(FEATURE(self), type, ENTITY_GET_ADDRESS(ENTITY_OBJECT(entity)), id, role);

  // Override "virtual functions table"
  FEATURE_REMOTE_INTERFACE(self) = &feature_remote_methods;

  self->entity             = entity;
  self->max_response_delay = kDefaultMaxResponseDelayMs;
}

FeatureRemoteObject* FeatureRemoteCreate(uint32_t id, EntityRemoteObject* entity, FeatureTypeType type, RoleType role) {
  FeatureRemote* const feature_remote = (FeatureRemote*)EEBUS_MALLOC(sizeof(FeatureRemote));

  FeatureRemoteConstruct(feature_remote, id, entity, type, role);

  return FEATURE_REMOTE_OBJECT(feature_remote);
}

void Destruct(FeatureObject* self) {
  FeatureDestruct(self);
}

DeviceRemoteObject* GetDevice(const FeatureRemoteObject* self) {
  const FeatureRemote* const fr = FEATURE_REMOTE(self);
  return ENTITY_REMOTE_GET_DEVICE(fr->entity);
}

EntityRemoteObject* GetEntity(const FeatureRemoteObject* self) {
  return FEATURE_REMOTE(self)->entity;
}

const void* GetData(const FeatureRemoteObject* self, FunctionType function_type) {
  const FeatureRemote* const fr = FEATURE_REMOTE(self);

  FunctionObject* const fcn = FeatureGetFunction(FEATURE(fr), function_type);
  if (fcn == NULL) {
    return NULL;
  }

  return FUNCTION_GET_DATA(fcn);
}

void* DataCopy(const FeatureRemoteObject* self, FunctionType fcn_type) {
  const FeatureRemote* const fr = FEATURE_REMOTE(self);

  FunctionObject* const fcn = FeatureGetFunction(FEATURE(fr), fcn_type);
  if (fcn == NULL) {
    return NULL;
  }

  return FUNCTION_DATA_COPY(fcn);
}

EebusError UpdateData(
    FeatureRemoteObject* self,
    FunctionType function_type,
    const void* new_data,
    const FilterType* filter_partial,
    const FilterType* filter_delete,
    bool persist
) {
  FunctionObject* const function = FeatureGetFunction(FEATURE(self), function_type);
  if (function == NULL) {
    return kEebusErrorInputArgument;
  }

  return FUNCTION_UPDATE_DATA(function, new_data, filter_partial, filter_delete, false, persist);
}

void SetOperations(FeatureRemoteObject* self, FunctionType function_type, const PossibleOperationsType* ops) {
  FunctionObject* const function = FeatureGetFunction(FEATURE(self), function_type);
  if (function == NULL) {
    return;
  }

  const bool read          = (ops->read != NULL);
  const bool read_partial  = (ops->read != NULL) && (ops->read->partial != EEBUS_TAG_RESET);
  const bool write         = (ops->write != NULL);
  const bool write_partial = (ops->write != NULL) && (ops->write->partial != EEBUS_TAG_RESET);

  FUNCTION_SET_OPERATIONS(function, read, read_partial, write, write_partial);
}

void SetFunctionOperations(
    FeatureRemoteObject* self,
    const FunctionPropertyType* const* supported_functions,
    size_t supported_functions_size
) {
  for (size_t i = 0; i < supported_functions_size; ++i) {
    if ((supported_functions[i]->possible_operations != NULL) && (supported_functions[i]->function != NULL)) {
      SetOperations(self, *supported_functions[i]->function, supported_functions[i]->possible_operations);
    }
  }
}

void SetMaxResponseDelay(FeatureRemoteObject* self, uint32_t max_delay) {
  FEATURE_REMOTE(self)->max_response_delay = max_delay;
}

uint32_t GetMaxResponseDelay(const FeatureRemoteObject* self) {
  return FEATURE_REMOTE(self)->max_response_delay;
}
