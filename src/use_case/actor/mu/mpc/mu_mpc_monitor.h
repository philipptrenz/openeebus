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
 * @brief MU MPC Monitor implementation declarations.
 *
 * Use following functions to create a monitor per scenario.
 * Scenario 1: MuMpcMonitorPowerCreate();
 * Scenario 2: MuMpcMonitorEnergyCreate();
 * Scenario 3: MuMpcMonitorCurrentCreate();
 * Scenario 4: MuMpcMonitorVoltageCreate();
 * Scenario 5: MuMpcMonitorFrequencyCreate().
 *
 * Use MuMpcMonitorDelete() to delete the created MU MPC Monitor instance
 */

#ifndef SRC_USE_CASE_ACTOR_MU_MU_MPC_MONITOR_H_
#define SRC_USE_CASE_ACTOR_MU_MU_MPC_MONITOR_H_

#include <stddef.h>

#include "src/common/eebus_malloc.h"
#include "src/use_case/actor/mu/mpc/mu_mpc_measurement.h"
#include "src/use_case/api/mu_mpc_monitor_interface.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * MuMpcMonitorCurrentConfig is the configuration for the monitor use case
 * If this config is passed via NewMPC, the use case will support current monitoring as specified
 */
typedef struct MuMpcMonitorCurrentConfig MuMpcMonitorCurrentConfig;

struct MuMpcMonitorCurrentConfig {
  /** Phase A AC Current measurement configuration */
  const MuMpcMeasurementConfig* current_phase_a_cfg;
  /** Phase B AC Current measurement configuration */
  const MuMpcMeasurementConfig* current_phase_b_cfg;
  /** Phase C AC Current measurement configuration */
  const MuMpcMeasurementConfig* current_phase_c_cfg;
};

/**
 * MuMpcMonitorEnergyConfig is the configuration for the monitor use case
 * If this config is passed via NewMPC, the use case will support energy monitoring as specified
 */
typedef struct MuMpcMonitorEnergyConfig MuMpcMonitorEnergyConfig;

struct MuMpcMonitorEnergyConfig {
  /** The source of the production values (if not NULL is set, the use case will support production) */
  const MuMpcMeasurementConfig* energy_production_cfg;

  /**< The source of the consumption values (if not NULL use case will support consumption) */
  const MuMpcMeasurementConfig* energy_consumption_cfg;
};

/**
 * MuMpcMonitorFrequencyConfig is the configuration for the monitor use case
 * If this config is passed via NewMPC, the use case will support frequency monitoring as specified
 */
typedef struct MuMpcMonitorFrequencyConfig MuMpcMonitorFrequencyConfig;

struct MuMpcMonitorFrequencyConfig {
  /** The source of the production values (if not NULL is set, the use case will support production) */
  MuMpcMeasurementConfig frequency_cfg;
};

/**
 * MuMpcMonitorPowerConfig is the configuration for the monitor use case
 * This config is required by the mpc use case and must be used in mpc.NewMPC
 */
typedef struct MuMpcMonitorPowerConfig MuMpcMonitorPowerConfig;

struct MuMpcMonitorPowerConfig {
  /** The source of the values from the ac_power_total (required) */
  MuMpcMeasurementConfig power_total_cfg;

  /** Phase A AC Power measurement configuration (required if the phase is supported) */
  const MuMpcMeasurementConfig* power_phase_a_cfg;
  /** Phase B AC Power measurement configuration (required if the phase is supported) */
  const MuMpcMeasurementConfig* power_phase_b_cfg;
  /** Phase C AC Power measurement configuration (required if the phase is supported) */
  const MuMpcMeasurementConfig* power_phase_c_cfg;
};

/**
 * MuMpcMonitorVoltageConfig is the configuration for the monitor use case
 * This config is required by the mpc use case and must be used in mpc.NewMPC
 */
typedef struct MuMpcMonitorVoltageConfig MuMpcMonitorVoltageConfig;

struct MuMpcMonitorVoltageConfig {
  /** Phase A AC Voltage measurement configuration (required if the phase is supported) */
  const MuMpcMeasurementConfig* voltage_phase_a_cfg;
  /** Phase B AC Voltage measurement configuration (required if the phase is supported) */
  const MuMpcMeasurementConfig* voltage_phase_b_cfg;
  /** Phase C AC Voltage measurement configuration (required if the phase is supported) */
  const MuMpcMeasurementConfig* voltage_phase_c_cfg;

  /**
   * Phase A to B measurement configuration
   * (can be used only if the both related phases are supported)
   */
  const MuMpcMeasurementConfig* voltage_phase_ab_cfg;

  /**
   * Phase B to C measurement configuration
   * (can be used only if the both related phases are supported)
   */
  const MuMpcMeasurementConfig* voltage_phase_bc_cfg;

  /**
   * Phase C to A measurement configuration
   * (can be used only if the both related phases are supported)
   */
  const MuMpcMeasurementConfig* voltage_phase_ac_cfg;
};

/**
 * @brief Creates a new MuMpcMonitorObject for the current monitoring (Scenario 1)
 * @param cfg The configuration for the current monitoring use case
 * @return A pointer to the created MuMpcMonitorObject, or NULL if creation failed
 */
MuMpcMonitorObject* MuMpcMonitorPowerCreate(const MuMpcMonitorPowerConfig* cfg);

/**
 * @brief Creates a new MuMpcMonitorObject for the energy monitoring (Scenario 2)
 * @param cfg The configuration for the energy monitoring use case
 * @return A pointer to the created MuMpcMonitorObject, or NULL if creation failed
 */
MuMpcMonitorObject* MuMpcMonitorEnergyCreate(const MuMpcMonitorEnergyConfig* cfg);

/**
 * @brief Creates a new MuMpcMonitorObject for the current monitoring (Scenario 3)
 * @param cfg The configuration for the current monitoring use case
 * @return A pointer to the created MuMpcMonitorObject, or NULL if creation failed
 */
MuMpcMonitorObject* MuMpcMonitorCurrentCreate(const MuMpcMonitorCurrentConfig* cfg);

/**
 * @brief Creates a new MuMpcMonitorObject for the voltage monitoring (Scenario 4)
 * @param cfg The configuration for the voltage monitoring use case
 * @return A pointer to the created MuMpcMonitorObject, or NULL if creation failed
 */
MuMpcMonitorObject* MuMpcMonitorVoltageCreate(const MuMpcMonitorVoltageConfig* cfg);

/**
 * @brief Creates a new MuMpcMonitorObject for the frequency monitoring (Scenario 5)
 * @param cfg The configuration for the frequency monitoring use case
 * @return A pointer to the created MuMpcMonitorObject, or NULL if creation failed
 */
MuMpcMonitorObject* MuMpcMonitorFrequencyCreate(const MuMpcMonitorFrequencyConfig* cfg);

/**
 * @brief Destructs the MuMpcMonitorObject instance and frees the memory
 * @param mu_mpc_monitor Pointer to the MuMpcMonitorObject instance to be deleted
 */
static inline void MuMpcMonitorDelete(MuMpcMonitorObject* mu_mpc_monitor) {
  if (mu_mpc_monitor != NULL) {
    MU_MPC_MONITOR_DESTRUCT(mu_mpc_monitor);
    EEBUS_FREE(mu_mpc_monitor);
  }
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_ACTOR_MU_MU_MPC_MONITOR_H_
