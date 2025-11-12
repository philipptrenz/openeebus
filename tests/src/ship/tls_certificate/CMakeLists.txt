# Tls Certificate integration test
cmake_minimum_required(VERSION 3.15)

set(TEST_NAME tls_certificate_test)

project(${TESTS_NAME} LANGUAGES C CXX)

add_executable(${TEST_NAME})

target_sources(
  ${TEST_NAME}
  PRIVATE

  # Main project sources
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c
  ${MAIN_PROJ_SOURCES_PATH}/ship/tls_certificate/tls_certificate.c

  tls_certificate_test.cpp
)

target_include_directories(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_INCLUDES_PATH}
)

target_compile_options(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_OPTIONS}
)

target_compile_definitions(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_COMPILE_DEFINITIONS}
)

target_link_options(
  ${TEST_NAME}
  PRIVATE
)

target_link_libraries(
  ${TEST_NAME}
  PRIVATE
  ssl
  crypto
)
