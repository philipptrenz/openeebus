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
 * @brief Energy Guard LPC use case internal declarations
 */

#ifndef SRC_USE_CASE_ENERGY_GUARD_LPC_EG_LPC_INTERNAL_H_
#define SRC_USE_CASE_ENERGY_GUARD_LPC_EG_LPC_INTERNAL_H_

#include "src/use_case/api/eg_lpc_listener_interface.h"
#include "src/use_case/use_case.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct EgLpcUseCase EgLpcUseCase;
struct EgLpcUseCase {
  /** Inherits the Entity */
  UseCase obj;

  EgLpcListenerObject* eg_lpc_listener;
};

#define EG_LPC_USE_CASE(obj) ((EgLpcUseCase*)(obj))

EebusError EgLpcGetActivePowerConsumptionLimitInternal(
    const EgLpcUseCase* self,
    const EntityAddressType* remote_entity_addr,
    LoadLimit* limit
);

EebusError EgLpcGetFailsafeConsumptionActivePowerLimitInternal(
    const EgLpcUseCase* self,
    const EntityAddressType* remote_entity_addr,
    ScaledValue* power_limit
);

EebusError EgLpcGetFailsafeDurationMinimumInternal(
    const EgLpcUseCase* self,
    const EntityAddressType* remote_entity_addr,
    DurationType* duration
);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ENERGY_GUARD_LPC_EG_LPC_INTERNAL_H_
