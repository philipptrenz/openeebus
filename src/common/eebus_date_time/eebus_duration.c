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
 * @brief Eebus Duration implementation
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/common/array_util.h"
#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/eebus_malloc.h"

static const char zero_duration[] = "PT0S";

// Constants for conversion
static const int32_t seconds_per_minute = 60;
static const int32_t seconds_per_hour   = 3600;      // 60 * 60
static const int32_t seconds_per_day    = 86400;     // 24 * 3600
static const int32_t seconds_per_month  = 2592000;   // 30 * 86400 (Approximation: 30 days per month)
static const int32_t seconds_per_year   = 31536000;  // 365 * 86400 (Approximation: 365 days per year)

static EebusError EebusDurationSetValue(EebusDuration* self, char key, bool is_time, int32_t value);
static size_t EebusDurationGetStringLength(const EebusDuration* self);

void EebusDurationInvertSign(EebusDuration* duration) {
  if (duration == NULL) {
    return;
  }

  // Invert the sign of each component
  duration->years   = -duration->years;
  duration->months  = -duration->months;
  duration->days    = -duration->days;
  duration->hours   = -duration->hours;
  duration->minutes = -duration->minutes;
  duration->seconds = -duration->seconds;
}

EebusError EebusDurationSetValue(EebusDuration* self, char key, bool is_time, int32_t value) {
  EebusError ret = kEebusErrorOk;

  if (is_time) {
    switch (key) {
      case 'H': self->hours = value; break;
      case 'M': self->minutes = value; break;
      case 'S': self->seconds = value; break;
      default: ret = kEebusErrorInputArgument; break;  // Invalid character
    }
  } else {
    switch (key) {
      case 'Y': self->years = value; break;
      case 'M': self->months = value; break;
      case 'D': self->days = value; break;
      default: ret = kEebusErrorInputArgument; break;  // Invalid character
    }
  }

  return ret;
}

EebusError EebusDurationParse(const char* s, EebusDuration* duration) {
  if ((s == NULL) || (duration == NULL)) {
    return kEebusErrorInputArgumentNull;
  }

  // Initialize the duration structure
  memset(duration, 0, sizeof(*duration));
  int32_t sign   = 1;  // Default to positive
  const char* ps = s;

  // Check for a leading '+' or '-' sign
  if (*ps == '-') {
    sign = -1;
    ++ps;
  } else if (*ps == '+') {
    ++ps;
  }

  // Check if the string starts with 'P'
  if (*ps != 'P') {
    return kEebusErrorParse;
  }

  ++ps;  // Skip the 'P'
  bool is_time = false;

  while (*ps != '\0') {
    if (*ps == 'T') {
      if (is_time) {
        return kEebusErrorParse;
      }

      is_time = true;
    } else {
      char* ps_end = NULL;

      int32_t value = strtol(ps, &ps_end, 10);
      if (ps == ps_end) {
        return kEebusErrorParse;  // No valid number found
      }

      ps = ps_end;
      if (EebusDurationSetValue(duration, *ps, is_time, value) != kEebusErrorOk) {
        return kEebusErrorParse;  // Invalid character
      }
    }

    ++ps;
  }

  if (sign == -1) {
    // If the sign is negative, negate the values
    EebusDurationInvertSign(duration);
  }

  return kEebusErrorOk;
}

bool EebusDurationIsZero(const EebusDuration* self) {
  if (self == NULL) {
    return false;
  }

  // Check if all components are zero
  return (self->years == 0) && (self->months == 0) && (self->days == 0) && (self->hours == 0) && (self->minutes == 0)
         && (self->seconds == 0);
}

bool EebusDurationIsNegative(const EebusDuration* self) {
  if (self == NULL) {
    return false;
  }

  if (EebusDurationIsZero(self)) {
    return false;  // Zero duration is not negative
  }

  // Check if the duration is negative
  return (self->years <= 0) && (self->months <= 0) && (self->days <= 0) && (self->hours <= 0) && (self->minutes <= 0)
         && (self->seconds <= 0);
}

bool EebusDurationIsPositive(const EebusDuration* self) {
  if (self == NULL) {
    return false;
  }

  if (EebusDurationIsZero(self)) {
    return false;  // Zero duration is not positive
  }

  // Check if the duration is positive
  return (self->years >= 0) && (self->months >= 0) && (self->days >= 0) && (self->hours >= 0) && (self->minutes >= 0)
         && (self->seconds >= 0);
}

bool EebusDurationIsValid(const EebusDuration* self) {
  if (self == NULL) {
    return false;
  }

  // Check if the duration is valid
  return EebusDurationIsZero(self) || EebusDurationIsNegative(self) || EebusDurationIsPositive(self);
}

size_t print_non_zero(char* buffer, size_t size, const char* format, int32_t number) {
  if (number == 0) {
    return 0;  // No number to print
  }

  if (buffer == NULL) {
    return snprintf(NULL, 0, format, number);
  } else {
    return snprintf(buffer, size, format, number);
  }
}

size_t EebusDurationGetStringLength(const EebusDuration* self) {
  if (self == NULL) {
    return 0;
  }

  if (EebusDurationIsZero(self)) {
    return ARRAY_SIZE(zero_duration);
  }

  // Calculate the size of the string representation
  size_t size = 2;  // For "P" and null terminator

  if (EebusDurationIsNegative(self)) {
    size += 1;  // For the sign
  }

  size += print_non_zero(NULL, 0, "%dY", self->years);
  size += print_non_zero(NULL, 0, "%dM", self->months);
  size += print_non_zero(NULL, 0, "%dD", self->days);

  if (self->hours > 0 || self->minutes > 0 || self->seconds > 0) {
    size += 1;  // For "T"
    size += print_non_zero(NULL, 0, "%dH", self->hours);
    size += print_non_zero(NULL, 0, "%dM", self->minutes);
    size += print_non_zero(NULL, 0, "%dS", self->seconds);
  }

  return size;
}

char* EebusDurationToString(const EebusDuration* self) {
  if ((self == NULL) || (!EebusDurationIsValid(self))) {
    return NULL;
  }

  size_t size  = EebusDurationGetStringLength(self) + 1;  // +1 for null terminator
  char* buffer = (char*)EEBUS_MALLOC(size);
  if (buffer == NULL) {
    return NULL;  // Memory allocation failed
  }

  if (EebusDurationIsZero(self)) {
    snprintf(buffer, size, zero_duration);
    return buffer;
  }

  // Start with the sign
  size_t offset = EebusDurationIsNegative(self) ? snprintf(buffer, size, "-") : 0;
  // Add the 'P' prefix
  offset += snprintf(buffer + offset, size - offset, "P");
  // Add the date components
  offset += print_non_zero(buffer + offset, size - offset, "%dY", self->years);
  offset += print_non_zero(buffer + offset, size - offset, "%dM", self->months);
  offset += print_non_zero(buffer + offset, size - offset, "%dD", self->days);

  // Add the time components if any exist
  if (self->hours > 0 || self->minutes > 0 || self->seconds > 0) {
    offset += snprintf(buffer + offset, size - offset, "T");
    offset += print_non_zero(buffer + offset, size - offset, "%dH", self->hours);
    offset += print_non_zero(buffer + offset, size - offset, "%dM", self->minutes);
    offset += print_non_zero(buffer + offset, size - offset, "%dS", self->seconds);
  }

  return buffer;
}

int64_t EebusDurationToSeconds(const EebusDuration* self) {
  if ((self == NULL) || (!EebusDurationIsValid(self))) {
    return 0;  // Return 0 for invalid input
  }

  // Calculate total seconds
  int64_t total_seconds = 0;
  total_seconds += (int64_t)self->years * seconds_per_year;
  total_seconds += (int64_t)self->months * seconds_per_month;
  total_seconds += (int64_t)self->days * seconds_per_day;
  total_seconds += (int64_t)self->hours * seconds_per_hour;
  total_seconds += (int64_t)self->minutes * seconds_per_minute;
  total_seconds += (int64_t)self->seconds;

  return total_seconds;
}

int32_t EebusDurationCompare(const EebusDuration* self, const EebusDuration* other) {
  if ((self == NULL) || (other == NULL) || (!EebusDurationIsValid(self)) || (!EebusDurationIsValid(other))) {
    return 0;  // Consider invalid durations as equal
  }

  // Compare each component in order of significance
  if (self->years != other->years) {
    return (self->years < other->years) ? -1 : 1;
  }

  if (self->months != other->months) {
    return (self->months < other->months) ? -1 : 1;
  }

  if (self->days != other->days) {
    return (self->days < other->days) ? -1 : 1;
  }

  if (self->hours != other->hours) {
    return (self->hours < other->hours) ? -1 : 1;
  }

  if (self->minutes != other->minutes) {
    return (self->minutes < other->minutes) ? -1 : 1;
  }

  if (self->seconds != other->seconds) {
    return (self->seconds < other->seconds) ? -1 : 1;
  }

  return 0;  // Durations are equal
}
