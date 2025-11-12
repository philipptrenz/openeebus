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
 * @brief EEBUS CLI implementation
 */

#include <stdio.h>

#include "src/cli/eebus_cli.h"
#include "src/common/array_util.h"
#include "src/common/eebus_errors.h"
#include "src/common/eebus_malloc.h"
#include "src/common/string_util.h"

typedef struct EebusCli EebusCli;

struct EebusCli {
  /** Implements the EEBUS CLI Interface */
  EebusCliObject obj;

  /** EG LPC instance to deal with */
  EgLpcUseCaseObject* eg_lpc;
  /** EG LPC remote entity address to communicate with */
  const EntityAddressType* eg_lpc_entity_addr;

  /** MA MPC instance to deal with */
  MaMpcUseCaseObject* ma_mpc;
  /** MA MPC remote entity address to communicate with */
  const EntityAddressType* ma_mpc_entity_addr;
};

#define EEBUS_CLI(obj) ((EebusCli*)(obj))

static void Destruct(EebusCliObject* self);
static void
SetEgLpc(EebusCliObject* self, EgLpcUseCaseObject* eg_lpc_use_case, const EntityAddressType* remote_entity_address);
static void
SetMaMpc(EebusCliObject* self, MaMpcUseCaseObject* ma_mpc_use_case, const EntityAddressType* remote_entity_address);
static void HandleCmd(const EebusCliObject* self, char* cmd);

static const EebusCliInterface eebus_cli_methods = {
    .destruct   = Destruct,
    .set_eg_lpc = SetEgLpc,
    .set_ma_mpc = SetMaMpc,
    .handle_cmd = HandleCmd,
};

static EebusError EebusCliConstruct(EebusCli* self);

EebusError EebusCliConstruct(EebusCli* self) {
  // Override "virtual functions table"
  EEBUS_CLI_INTERFACE(self) = &eebus_cli_methods;

  self->eg_lpc             = NULL;
  self->eg_lpc_entity_addr = NULL;

  self->ma_mpc             = NULL;
  self->ma_mpc_entity_addr = NULL;

  return kEebusErrorOk;
}

EebusCliObject* EebusCliCreate(void) {
  EebusCli* const eebus_cli = (EebusCli*)EEBUS_MALLOC(sizeof(EebusCli));
  if (eebus_cli == NULL) {
    return NULL;
  }

  if (EebusCliConstruct(eebus_cli) != kEebusErrorOk) {
    EebusCliDelete(EEBUS_CLI_OBJECT(eebus_cli));
    return NULL;
  }

  return EEBUS_CLI_OBJECT(eebus_cli);
}

void Destruct(EebusCliObject* self) {
  EebusCli* const eebus_cli = EEBUS_CLI(self);

  EntityAddressDelete((EntityAddressType*)eebus_cli->eg_lpc_entity_addr);
  eebus_cli->eg_lpc_entity_addr = NULL;

  EntityAddressDelete((EntityAddressType*)eebus_cli->ma_mpc_entity_addr);
  eebus_cli->ma_mpc_entity_addr = NULL;
}

void SetEgLpc(
    EebusCliObject* self,
    EgLpcUseCaseObject* eg_lpc_use_case,
    const EntityAddressType* remote_entity_address
) {
  EebusCli* const eebus_cli = EEBUS_CLI(self);

  eebus_cli->eg_lpc = eg_lpc_use_case;

  // Release the old entity adress
  if (eebus_cli->eg_lpc_entity_addr != NULL) {
    EntityAddressDelete((EntityAddressType*)eebus_cli->eg_lpc_entity_addr);
    eebus_cli->eg_lpc_entity_addr = NULL;
  }

  // Copy the new entity address if not NULL
  if (remote_entity_address != NULL) {
    eebus_cli->eg_lpc_entity_addr = EntityAddressCopy(remote_entity_address);
  }
}

void SetMaMpc(
    EebusCliObject* self,
    MaMpcUseCaseObject* ma_mpc_use_case,
    const EntityAddressType* remote_entity_address
) {
  EebusCli* const eebus_cli = EEBUS_CLI(self);

  eebus_cli->ma_mpc = ma_mpc_use_case;

  // Release the old entity adress
  if (eebus_cli->ma_mpc_entity_addr != NULL) {
    EntityAddressDelete((EntityAddressType*)eebus_cli->ma_mpc_entity_addr);
    eebus_cli->ma_mpc_entity_addr = NULL;
  }

  // Copy the new entity address if not NULL
  if (remote_entity_address != NULL) {
    eebus_cli->ma_mpc_entity_addr = EntityAddressCopy(remote_entity_address);
  }
}

//-------------------------------------------------------------------------------------------//
//
// EG LPC Getters Handling
//
//-------------------------------------------------------------------------------------------//
void EebusCliHandleCmdEgLpcGetPowerLimit(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  // Example:
  // eg_lpc get power_limit
  LoadLimit limit = {0};
  if (EgLpcGetActivePowerConsumptionLimit(self->eg_lpc, self->eg_lpc_entity_addr, &limit) != kEebusErrorOk) {
    printf("Getting power limit failed\n");
    return;
  }

  printf(
      "Power Limit: value=%lld, scale=%d, duration=%dh, active=%s\n",
      (long long)limit.value.value,
      (int)limit.value.scale,
      (int)limit.duration.hours,
      limit.is_active ? "true" : "false"
  );
}

void EebusCliHandleCmdEgLpcGetFailsafeLimit(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  ScaledValue power_limit = {0};

  if (EgLpcGetFailsafeConsumptionActivePowerLimit(self->eg_lpc, self->eg_lpc_entity_addr, &power_limit)
      != kEebusErrorOk) {
    printf("Getting failsafe limit failed\n");
    return;
  }

  printf(
      "Failsafe Consumption Active Power Limit: value=%lld, scale=%d\n",
      (long long)power_limit.value,
      power_limit.scale
  );
}

void EebusCliHandleCmdEgLpcGetFailsafeDuration(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  DurationType duration = {0};

  if (num_tokens != 3) {
    printf("Insufficient arguments for eg_lpc get failsafe_duration command\n");
    return;
  }

  if (EgLpcGetFailsafeDurationMinimum(self->eg_lpc, self->eg_lpc_entity_addr, &duration) != kEebusErrorOk) {
    printf("Getting failsafe duration failed\n");
    return;
  }

  printf(
      "Failsafe Duration Minimum: %dy %dm %dd %dh %dm %ds\n",
      (int)duration.years,
      (int)duration.days,
      (int)duration.months,
      (int)duration.hours,
      (int)duration.minutes,
      (int)duration.seconds
  );
}

void EebusCliHandleCmdEgLpcGet(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for eg_lpc get command\n");
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_limit") == 0) {
    EebusCliHandleCmdEgLpcGetPowerLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_limit") == 0) {
    EebusCliHandleCmdEgLpcGetFailsafeLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_duration") == 0) {
    EebusCliHandleCmdEgLpcGetFailsafeDuration(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for eg_lpc get: %s\n", subcommand);
  }
}

//-------------------------------------------------------------------------------------------//
//
// EG LPC Setters Handling
//
//-------------------------------------------------------------------------------------------//
void EebusCliHandleCmdEgLpcSetPowerLimit(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  // Example:
  // eg_lpc set power_limit 35005 -1 60 true
  // eg_lpc set power_limit 35005 -2 12 false
  if (num_tokens < 7) {
    printf("Insufficient arguments for eg_lpc set power_limit command\n");
    return;
  }

  LoadLimit limit = {0};
  char* endptr;

  limit.value.value = strtol(tokens[3], &endptr, 10);
  if (*endptr != '\0') {
    printf("Invalid limit value: %s\n", tokens[3]);
    return;
  }

  limit.value.scale = strtol(tokens[4], &endptr, 10);
  if (*endptr != '\0') {
    printf("Invalid scale value: %s\n", tokens[4]);
    return;
  }

  if (EebusDurationParse(tokens[5], &limit.duration) != kEebusErrorOk) {
    printf("Invalid duration value: %s\n", tokens[5]);
    return;
  }

  if (strcmp(tokens[6], "true") == 0) {
    limit.is_active = true;
  } else if (strcmp(tokens[6], "false") == 0) {
    limit.is_active = false;
  } else {
    printf("Invalid active flag value: %s\n", tokens[6]);
    return;
  }

  if (EgLpcSetActivePowerConsumptionLimit(self->eg_lpc, self->eg_lpc_entity_addr, &limit) != kEebusErrorOk) {
    printf("Setting power limit failed\n");
  }
}

void EebusCliHandleCmdEgLpcSetFailsafeLimit(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  ScaledValue power_limit = {0};
  char* endptr;

  if (num_tokens != 5) {
    printf("Insufficient arguments for eg_lpc set failsafe_limit command\n");
    return;
  }

  power_limit.value = strtol(tokens[3], &endptr, 10);
  if (*endptr != '\0') {
    printf("Invalid value for failsafe_limit: %s\n", tokens[3]);
    return;
  }

  power_limit.scale = strtol(tokens[4], &endptr, 10);
  if (*endptr != '\0') {
    printf("Invalid scale for failsafe_limit: %s\n", tokens[4]);
    return;
  }

  if (EgLpcSetFailsafeConsumptionActivePowerLimit(self->eg_lpc, self->eg_lpc_entity_addr, &power_limit)
      != kEebusErrorOk) {
    printf("Setting failsafe limit failed\n");
  }
}

void EebusCliHandleCmdEgLpcSetFailsafeDuration(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  DurationType duration = {0};

  // eg_lpc set failsafe_duration 2
  if (num_tokens != 4) {
    printf("Insufficient arguments for eg_lpc set failsafe_duration command\n");
    return;
  }

  if (EebusDurationParse(tokens[3], &duration) != kEebusErrorOk) {
    printf("Invalid value for failsafe_duration: %s\n", tokens[3]);
    return;
  }

  if (EgLpcSetFailsafeDurationMinimum(self->eg_lpc, self->eg_lpc_entity_addr, &duration) != kEebusErrorOk) {
    printf("Setting failsafe duration failed\n");
  }
}

void EebusCliHandleCmdEgLpcSet(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 3) {
    printf("Insufficient arguments for eg_lpc set command\n");
    return;
  }

  const char* subcommand = tokens[2];
  if (strcmp(subcommand, "power_limit") == 0) {
    EebusCliHandleCmdEgLpcSetPowerLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_limit") == 0) {
    EebusCliHandleCmdEgLpcSetFailsafeLimit(self, tokens, num_tokens);
  } else if (strcmp(subcommand, "failsafe_duration") == 0) {
    EebusCliHandleCmdEgLpcSetFailsafeDuration(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for eg_lpc set: %s\n", subcommand);
  }
}

//-------------------------------------------------------------------------------------------//
//
// EG LPC Start/Stop Handling
//
//-------------------------------------------------------------------------------------------//
void EebusCliHandleCmdEgLpcStart(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  const char* const subcommand = tokens[2];

  if (strcmp(subcommand, "heartbeat") == 0) {
    EgLpcStartHeartbeat(self->eg_lpc);
    printf("EG LPC Heartbeat started\n");
  } else {
    printf("Unknown subcommand for eg_lpc start: %s\n", subcommand);
  }
}

void EebusCliHandleCmdEgLpcStop(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  const char* const subcommand = tokens[2];

  if (strcmp(subcommand, "heartbeat") == 0) {
    EgLpcStopHeartbeat(self->eg_lpc);
    printf("EG LPC Heartbeat stopped\n");
  } else {
    printf("Unknown subcommand for eg_lpc stop: %s\n", subcommand);
  }
}

void EebusCliHandleCmdEgLpc(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 2) {
    printf("Insufficient arguments for eg_lpc command\n");
    return;
  }

  if (self->eg_lpc == NULL) {
    printf("EG LPC Use Case not set in CLI handler\n");
    return;
  }

  if (strcmp(tokens[1], "set") == 0) {
    EebusCliHandleCmdEgLpcSet(self, tokens, num_tokens);
  } else if (strcmp(tokens[1], "get") == 0) {
    EebusCliHandleCmdEgLpcGet(self, tokens, num_tokens);
  } else if (strcmp(tokens[1], "start") == 0) {
    EebusCliHandleCmdEgLpcStart(self, tokens, num_tokens);
  } else if (strcmp(tokens[1], "stop") == 0) {
    EebusCliHandleCmdEgLpcStop(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for eg_lpc: %s\n", tokens[1]);
  }
}

//-------------------------------------------------------------------------------------------//
//
// MA MPC Getters Handling
//
//-------------------------------------------------------------------------------------------//
void EebusCliHandleCmdMaMpcGet(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens != 3) {
    printf("Insufficient arguments for ma_mpc get command\n");
    return;
  }

  typedef struct MpcNameMapping {
    const char* name;
    MuMpcMeasurementNameId id;
  } MpcNameMapping;

  MpcNameMapping mpc_name_lut[] = {
      {     "power_total",     kMpcPowerTotal},
      {   "power_phase_a",    kMpcPowerPhaseA},
      {   "power_phase_b",    kMpcPowerPhaseB},
      {   "power_phase_c",    kMpcPowerPhaseC},
      { "energy_consumed", kMpcEnergyConsumed},
      { "energy_produced", kMpcEnergyProduced},
      {  "current_phasea",  kMpcCurrentPhaseA},
      {  "current_phaseb",  kMpcCurrentPhaseB},
      {  "current_phasec",  kMpcCurrentPhaseC},
      { "voltage_phase_a",  kMpcVoltagePhaseA},
      { "voltage_phase_b",  kMpcVoltagePhaseB},
      { "voltage_phase_c",  kMpcVoltagePhaseC},
      {"voltage_phase_ab", kMpcVoltagePhaseAb},
      {"voltage_phase_bc", kMpcVoltagePhaseBc},
      {"voltage_phase_ac", kMpcVoltagePhaseAc},
      {       "frequency",      kMpcFrequency},
  };

  const char* const name                = tokens[2];
  const MuMpcMeasurementNameId* name_id = NULL;
  for (size_t i = 0; i < ARRAY_SIZE(mpc_name_lut); i++) {
    if (strcmp(name, mpc_name_lut[i].name) == 0) {
      name_id = &mpc_name_lut[i].id;
      break;
    }
  }

  if (name_id == NULL) {
    printf("Unknown measurement name for ma_mpc get: %s\n", name);
    return;
  }

  ScaledValue value = {0};
  if (MaMpcGetMeasurementData(self->ma_mpc, *name_id, self->ma_mpc_entity_addr, &value) != kEebusErrorOk) {
    printf("Getting measurement value failed\n");
    return;
  }

  printf("Measurement %s: value=%lld, scale=%d\n", name, (long long)value.value, (int)value.scale);
}

void EebusCliHandleCmdMaMpc(const EebusCli* self, const char* const* tokens, size_t num_tokens) {
  if (num_tokens < 2) {
    printf("Insufficient arguments for ma_mpc command\n");
    return;
  }

  if (self->ma_mpc == NULL) {
    printf("MA MPC Use Case not set in CLI handler\n");
    return;
  }

  if (strcmp(tokens[1], "get") == 0) {
    EebusCliHandleCmdMaMpcGet(self, tokens, num_tokens);
  } else {
    printf("Unknown subcommand for ma_mpc: %s\n", tokens[1]);
  }
}

void HandleCmd(const EebusCliObject* self, char* cmd) {
  const EebusCli* const eebus_cli = EEBUS_CLI(self);

  const char* tokens[7] = {0};
  size_t num_tokens     = 0;

  static const char delimiters[] = " \t\n";

  char* p = NULL;

  tokens[num_tokens++] = StringToken(cmd, delimiters, &p);
  if (tokens[0] == NULL) {
    return;
  }

  while (num_tokens < 7) {
    char* const token = StringToken(NULL, delimiters, &p);
    if (token == NULL) {
      break;
    }

    if (num_tokens >= ARRAY_SIZE(tokens)) {
      printf("Too many arguments specified!\n");
      break;
    }

    tokens[num_tokens++] = token;
  }

  if (strcmp(tokens[0], "eg_lpc") == 0) {
    EebusCliHandleCmdEgLpc(eebus_cli, tokens, num_tokens);
  } else if (strcmp(tokens[0], "ma_mpc") == 0) {
    EebusCliHandleCmdMaMpc(eebus_cli, tokens, num_tokens);
  } else {
    printf("Unknown command: %s\n", tokens[0]);
    return;
  }
}
