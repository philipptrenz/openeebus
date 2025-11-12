cmake_minimum_required(VERSION 3.15)

set(TEST_NAME eebus_data_test)

project(${TESTS_NAME} LANGUAGES C CXX)

add_executable(${TEST_NAME})

# Set proper runtime library for Windows to avoid LIBCMT conflicts
if(WIN32)
  set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
endif()

target_sources(
  ${TEST_NAME}
  PRIVATE
  ${GTEST_SOURCES}

  # Main project sources
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_base.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_bool.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_choice.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_choice_root.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_container.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_enum.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_list.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_numeric.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_sequence.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_simple.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_string.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_stub.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_tag.c
  ${MAIN_PROJ_SOURCES_PATH}/common/eebus_data/eebus_data_util.c
  ${MAIN_PROJ_SOURCES_PATH}/common/json_impl_cjson.c
  ${MAIN_PROJ_SOURCES_PATH}/common/string_util.c

  # Test helpers
  ${CMAKE_SOURCE_DIR}/src/string_ptr.cpp

  employee.c
  person.c
  somebody.c
  info.c
  address_test_data.cpp
  eebus_data_choice_test.cpp
  eebus_data_employee_test.cpp
  eebus_data_list_test.cpp
  eebus_data_person_test.cpp
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
  ${PROJECT_LINK_OPTIONS}
)

target_link_libraries(
  ${TEST_NAME}
  PRIVATE
  ${PROJECT_LINK_LIBRARIES}
  cjson
)

add_test(
  NAME
  ${TEST_NAME}
  COMMAND
  ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME}
)

gtest_discover_tests(${TEST_NAME})
