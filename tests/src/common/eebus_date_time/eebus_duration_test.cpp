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
#include <gtest/gtest.h>

#include "src/common/eebus_date_time/eebus_duration.h"
#include "src/common/string_util.h"

//-------------------------------------------------------------------------------------------//
//
// EebusDurationInvertSign() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDurationTest, EebusDurationInvertSignPositiveToNegative) {
  EebusDuration duration = {.hours = 1, .minutes = 30, .seconds = 15};  // Positive duration
  EebusDurationInvertSign(&duration);
  EXPECT_EQ(duration.hours, -1);
  EXPECT_EQ(duration.minutes, -30);
  EXPECT_EQ(duration.seconds, -15);
}

TEST(EebusDurationTest, EebusDurationInvertSignNegativeToPositive) {
  EebusDuration duration = {.hours = -1, .minutes = -30, .seconds = -15};  // Negative duration
  EebusDurationInvertSign(&duration);
  EXPECT_EQ(duration.hours, 1);
  EXPECT_EQ(duration.minutes, 30);
  EXPECT_EQ(duration.seconds, 15);
}

TEST(EebusDurationTest, EebusDurationInvertSignZeroDuration) {
  EebusDuration duration = {.hours = 0, .minutes = 0, .seconds = 0};  // Zero duration
  EebusDurationInvertSign(&duration);
  EXPECT_EQ(duration.hours, 0);
  EXPECT_EQ(duration.minutes, 0);
  EXPECT_EQ(duration.seconds, 0);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDurationIsValid() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDurationTest, EebusDurationIsValidAllZero) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = 0, .seconds = 0};  // All fields are zero
  EXPECT_TRUE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidAllPositive) {
  static constexpr EebusDuration duration = {.hours = 1, .minutes = 30, .seconds = 15};  // All fields are positive
  EXPECT_TRUE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidAllNegative) {
  static constexpr EebusDuration duration = {.hours = -1, .minutes = -30, .seconds = -15};  // All fields are negative
  EXPECT_TRUE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidMixedPositiveAndNegative) {
  static constexpr EebusDuration duration
      = {.hours = 1, .minutes = -30, .seconds = 15};  // Mixed positive and negative fields
  EXPECT_FALSE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidMixedNegativeAndPositive) {
  static constexpr EebusDuration duration
      = {.hours = -1, .minutes = 30, .seconds = -15};  // Mixed negative and positive fields
  EXPECT_FALSE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidZeroAndPositive) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = 30, .seconds = 15};  // Zero and positive fields
  EXPECT_TRUE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidZeroAndNegative) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = -30, .seconds = -15};  // Zero and negative fields
  EXPECT_TRUE(EebusDurationIsValid(&duration));
}

TEST(EebusDurationTest, EebusDurationIsValidZeroOnly) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = 0, .seconds = 0};  // All fields are zero
  EXPECT_TRUE(EebusDurationIsValid(&duration));
}

//-------------------------------------------------------------------------------------------//
//
// EebusDurationParse() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDurationTest, EebusDurationParseValidInput) {
  EebusDuration duration;
  EXPECT_EQ(EebusDurationParse("PT1H30M15S", &duration), kEebusErrorOk);  // 1 hour, 30 minutes, 15 seconds
  EXPECT_EQ(duration.hours, 1);
  EXPECT_EQ(duration.minutes, 30);
  EXPECT_EQ(duration.seconds, 15);
}

TEST(EebusDurationTest, EebusDurationParseInvalidInput) {
  EebusDuration duration;
  EXPECT_EQ(EebusDurationParse("InvalidDuration", &duration), kEebusErrorParse);
}

TEST(EebusDurationTest, EebusDurationParseEmptyInput) {
  EebusDuration duration;
  EXPECT_EQ(EebusDurationParse("", &duration), kEebusErrorParse);
}

TEST(EebusDurationTest, EebusDurationParseOnlySeconds) {
  EebusDuration duration;
  EXPECT_EQ(EebusDurationParse("PT45S", &duration), kEebusErrorOk);  // 45 seconds
  EXPECT_EQ(duration.hours, 0);
  EXPECT_EQ(duration.minutes, 0);
  EXPECT_EQ(duration.seconds, 45);
}

TEST(EebusDurationTest, EebusDurationParseOnlyMinutes) {
  EebusDuration duration;
  EXPECT_EQ(EebusDurationParse("PT20M", &duration), kEebusErrorOk);  // 20 minutes
  EXPECT_EQ(duration.hours, 0);
  EXPECT_EQ(duration.minutes, 20);
  EXPECT_EQ(duration.seconds, 0);
}

TEST(EebusDurationTest, EebusDurationParseOnlyHours) {
  EebusDuration duration;
  EXPECT_EQ(EebusDurationParse("PT2H", &duration), kEebusErrorOk);  // 2 hours
  EXPECT_EQ(duration.hours, 2);
  EXPECT_EQ(duration.minutes, 0);
  EXPECT_EQ(duration.seconds, 0);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDurationToString() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDurationTest, EebusDurationToStringFullDuration) {
  static constexpr EebusDuration duration
      = {.hours = 1, .minutes = 30, .seconds = 15};  // 1 hour, 30 minutes, 15 seconds

  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusDurationToString(&duration), &StringDelete);
  EXPECT_STREQ(s.get(), "PT1H30M15S");
}

TEST(EebusDurationTest, EebusDurationToStringOnlySeconds) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = 0, .seconds = 45};  // 45 seconds
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusDurationToString(&duration), &StringDelete);
  EXPECT_STREQ(s.get(), "PT45S");
}

TEST(EebusDurationTest, EebusDurationToStringOnlyMinutes) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = 20, .seconds = 0};  // 20 minutes
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusDurationToString(&duration), &StringDelete);
  EXPECT_STREQ(s.get(), "PT20M");
}

TEST(EebusDurationTest, EebusDurationToStringOnlyHours) {
  static constexpr EebusDuration duration = {.hours = 2, .minutes = 0, .seconds = 0};  // 2 hours
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusDurationToString(&duration), &StringDelete);
  EXPECT_STREQ(s.get(), "PT2H");
}

TEST(EebusDurationTest, EebusDurationToStringZeroDuration) {
  static constexpr EebusDuration duration = {.hours = 0, .minutes = 0, .seconds = 0};  // 0 duration
  std::unique_ptr<char[], decltype(&StringDelete)> s(EebusDurationToString(&duration), &StringDelete);
  EXPECT_STREQ(s.get(), "PT0S");
}

//-------------------------------------------------------------------------------------------//
//
// EebusDurationToSeconds() test
//
//-------------------------------------------------------------------------------------------//
TEST(EebusDurationTest, EebusDurationToSecondsFullDuration) {
  static constexpr EebusDuration duration_positive
      = {.hours = 1, .minutes = 30, .seconds = 15};             // 1 hour, 30 minutes, 15 seconds
  EXPECT_EQ(EebusDurationToSeconds(&duration_positive), 5415);  // 1*3600 + 30*60 + 15
  static constexpr EebusDuration duration_negative
      = {.hours = -1, .minutes = -30, .seconds = -15};           // - (1 hour, 30 minutes, 15 seconds)
  EXPECT_EQ(EebusDurationToSeconds(&duration_negative), -5415);  // -(1*3600 + 30*60 + 15)
}

TEST(EebusDurationTest, EebusDurationToSecondsOnlySeconds) {
  static constexpr EebusDuration duration_positive = {.hours = 0, .minutes = 0, .seconds = 45};  // 45 seconds
  EXPECT_EQ(EebusDurationToSeconds(&duration_positive), 45);
  static constexpr EebusDuration duration_negative = {.hours = 0, .minutes = 0, .seconds = -45};  // -45 seconds
  EXPECT_EQ(EebusDurationToSeconds(&duration_negative), -45);                                     // -45 seconds
}

TEST(EebusDurationTest, EebusDurationToSecondsOnlyMinutes) {
  static constexpr EebusDuration duration_positive = {.hours = 0, .minutes = 20, .seconds = 0};   // 20 minutes
  EXPECT_EQ(EebusDurationToSeconds(&duration_positive), 1200);                                    // 20*60
  static constexpr EebusDuration duration_negative = {.hours = 0, .minutes = -20, .seconds = 0};  // -20 minutes
  EXPECT_EQ(EebusDurationToSeconds(&duration_negative), -1200);                                   // -(20*60)
}

TEST(EebusDurationTest, EebusDurationToSecondsOnlyHours) {
  static constexpr EebusDuration duration_positive = {.hours = 2, .minutes = 0, .seconds = 0};   // 2 hours
  EXPECT_EQ(EebusDurationToSeconds(&duration_positive), 7200);                                   // 2*3600
  static constexpr EebusDuration duration_negative = {.hours = -2, .minutes = 0, .seconds = 0};  // -2 hours
  EXPECT_EQ(EebusDurationToSeconds(&duration_negative), -7200);                                  // -(2*3600)
}

TEST(EebusDurationTest, EebusDurationToSecondsZeroDuration) {
  static constexpr EebusDuration duration_zero = {.hours = 0, .minutes = 0, .seconds = 0};  // 0 duration
  EXPECT_EQ(EebusDurationToSeconds(&duration_zero), 0);
}

//-------------------------------------------------------------------------------------------//
//
// EebusDurationCompare() test
//
//-------------------------------------------------------------------------------------------//

TEST(EebusDurationTest, EebusDurationCompare) {
  // Same durations
  static constexpr EebusDuration duration1
      = {.years = 0, .months = 0, .days = 0, .hours = 1, .minutes = 30, .seconds = 15};
  static constexpr EebusDuration duration2
      = {.years = 0, .months = 0, .days = 0, .hours = 1, .minutes = 30, .seconds = 15};
  EXPECT_EQ(EebusDurationCompare(&duration1, &duration2), 0);

  // Different years
  static constexpr EebusDuration duration_year
      = {.years = 1, .months = 0, .days = 0, .hours = 0, .minutes = 0, .seconds = 0};
  EXPECT_GT(EebusDurationCompare(&duration_year, &duration1), 0);
  EXPECT_LT(EebusDurationCompare(&duration1, &duration_year), 0);

  // Different months
  static constexpr EebusDuration duration_month
      = {.years = 0, .months = 1, .days = 0, .hours = 0, .minutes = 0, .seconds = 0};
  EXPECT_GT(EebusDurationCompare(&duration_month, &duration1), 0);
  EXPECT_LT(EebusDurationCompare(&duration1, &duration_month), 0);

  // Different days
  static constexpr EebusDuration duration_day
      = {.years = 0, .months = 0, .days = 1, .hours = 0, .minutes = 0, .seconds = 0};
  EXPECT_GT(EebusDurationCompare(&duration_day, &duration1), 0);
  EXPECT_LT(EebusDurationCompare(&duration1, &duration_day), 0);

  // Different hours
  static constexpr EebusDuration duration_hours
      = {.years = 0, .months = 0, .days = 0, .hours = 2, .minutes = 0, .seconds = 0};
  EXPECT_GT(EebusDurationCompare(&duration_hours, &duration1), 0);
  EXPECT_LT(EebusDurationCompare(&duration1, &duration_hours), 0);

  // Different minutes
  static constexpr EebusDuration duration_minutes
      = {.years = 0, .months = 0, .days = 0, .hours = 1, .minutes = 45, .seconds = 0};
  EXPECT_GT(EebusDurationCompare(&duration_minutes, &duration1), 0);
  EXPECT_LT(EebusDurationCompare(&duration1, &duration_minutes), 0);

  // Different seconds
  static constexpr EebusDuration duration_seconds
      = {.years = 0, .months = 0, .days = 0, .hours = 1, .minutes = 30, .seconds = 30};
  EXPECT_GT(EebusDurationCompare(&duration_seconds, &duration1), 0);
  EXPECT_LT(EebusDurationCompare(&duration1, &duration_seconds), 0);
}
