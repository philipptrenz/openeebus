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
 * @brief Use Case data types eclarations
 */

#ifndef SRC_USE_CASE_API_TYPES_H_
#define SRC_USE_CASE_API_TYPES_H_

#include <stdbool.h>
#include <stdint.h>

#include "src/spine/model/common_data_types.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/**
 * @brief Scaled Value is similar to SPINE ScaledNumber but uses values instead of pointers internally.
 * Transformation of Scaled Value to folating point number is: n = value * 10 ^ scale
 */
typedef struct ScaledValue ScaledValue;

/**
 * @brief Scaled Value structure
 */
struct ScaledValue {
  int64_t value;  /**< Value */
  int8_t scale;   /**< Scale */
};

/**
 * @brif Load Control Limit (LPC) data
 */
typedef struct LoadLimit LoadLimit;

struct LoadLimit {
  ScaledValue value;     /**< Limit value */
  DurationType duration; /**< Duration of the limit */
  bool is_changeable;    /**< If the value can be changed via write, ignored when writing data */
  bool is_active;        /**< If the limit is active */
  bool delete_duration;  /**< If the Duration (TimePeriod in SPINE) should be deleted (only used for write commands.
                            Relevant for LPC & LPP only) */
};

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // SRC_USE_CASE_API_TYPES_H_
