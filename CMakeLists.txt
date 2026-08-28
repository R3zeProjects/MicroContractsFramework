cmake_minimum_required(VERSION 3.25)

project(MicroContractsFramework VERSION 0.12.0 LANGUAGES CXX)

set(MICRO_CONTRACTS_FRAMEWORK_PRERELEASE "beta")

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
find_package(Threads REQUIRED)

option(MCF_BUILD_EXAMPLES "Build MCF usage examples" ON)
option(MCF_BUILD_BENCHMARKS "Build MCF testing-utility benchmarks" OFF)

add_library(MicroContractsFramework INTERFACE)
add_library(vosp::contracts ALIAS MicroContractsFramework)
set_target_properties(MicroContractsFramework PROPERTIES EXPORT_NAME contracts)
target_compile_features(MicroContractsFramework INTERFACE cxx_std_23)
target_include_directories(MicroContractsFramework INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_options(MicroContractsFramework INTERFACE
    $<BUILD_INTERFACE:$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4;/permissive->>
    $<BUILD_INTERFACE:$<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-Wall;-Wextra;-Wpedantic>>
)

add_library(MicroTestingFramework INTERFACE)
add_library(vosp::testing ALIAS MicroTestingFramework)
set_target_properties(MicroTestingFramework PROPERTIES EXPORT_NAME testing)
target_link_libraries(MicroTestingFramework INTERFACE
    MicroContractsFramework
    Threads::Threads)

if(BUILD_TESTING)
    include(cmake/VospTesting.cmake)

    add_library(MicroContractsFrameworkHeaderContracts OBJECT
        tests/header_contracts/configuration_header.cpp
        tests/header_contracts/cache_header.cpp
        tests/header_contracts/error_header.cpp
        tests/header_contracts/logging_header.cpp
        tests/header_contracts/plugin_header.cpp
        tests/header_contracts/protocol_header.cpp
        tests/header_contracts/security_header.cpp
        tests/header_contracts/telemetry_header.cpp
        tests/header_contracts/transport_header.cpp
        tests/header_contracts/testing_header.cpp)
    target_link_libraries(MicroContractsFrameworkHeaderContracts
        PRIVATE MicroContractsFramework)

    add_executable(MicroContractsFrameworkTests tests/contracts_tests.cpp)
    target_link_libraries(MicroContractsFrameworkTests PRIVATE MicroContractsFramework)
    add_test(NAME MicroContractsFramework.Tests COMMAND MicroContractsFrameworkTests)

    add_executable(MicroContractsFrameworkTestingTests tests/testing_tests.cpp)
    target_link_libraries(MicroContractsFrameworkTestingTests PRIVATE MicroTestingFramework)
    add_test(NAME MicroContractsFramework.Testing COMMAND MicroContractsFrameworkTestingTests)

    vosp_add_compile_fail_test(
        NAME MicroContractsFramework.CompileFail.Error
        SOURCES tests/compile_fail/invalid_error.cpp
        LINK_LIBRARIES MicroContractsFramework)
    vosp_add_compile_fail_test(
        NAME MicroContractsFramework.CompileFail.Plugin
        SOURCES tests/compile_fail/invalid_plugin.cpp
        LINK_LIBRARIES MicroContractsFramework)
endif()

if(MCF_BUILD_EXAMPLES)
    add_executable(MicroContractsFrameworkExample examples/basic.cpp)
    target_link_libraries(MicroContractsFrameworkExample PRIVATE MicroContractsFramework)
    add_executable(MicroContractsFrameworkContractsExample examples/contracts.cpp)
    target_link_libraries(MicroContractsFrameworkContractsExample PRIVATE MicroContractsFramework)
    add_executable(MicroContractsFrameworkTestingExample examples/testing.cpp)
    target_link_libraries(MicroContractsFrameworkTestingExample PRIVATE MicroTestingFramework)
    if(BUILD_TESTING)
        add_test(NAME MicroContractsFramework.Example.Basic
            COMMAND MicroContractsFrameworkExample)
        add_test(NAME MicroContractsFramework.Example.Contracts
            COMMAND MicroContractsFrameworkContractsExample)
        add_test(NAME MicroContractsFramework.Example.Testing
            COMMAND MicroContractsFrameworkTestingExample)
    endif()
endif()

if(MCF_BUILD_BENCHMARKS)
    add_executable(MicroContractsFrameworkTestingBenchmark benchmarks/testing_benchmark.cpp)
    target_link_libraries(MicroContractsFrameworkTestingBenchmark PRIVATE MicroTestingFramework)
endif()

install(TARGETS MicroContractsFramework MicroTestingFramework
    EXPORT vosp_contractsTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES LICENSE
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/MicroContractsFramework)
install(EXPORT vosp_contractsTargets
    FILE vosp_contractsTargets.cmake
    NAMESPACE vosp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vosp_contracts
)
configure_package_config_file(
    cmake/vosp_contractsConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/vosp_contractsConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vosp_contracts
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/vosp_contractsConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/vosp_contractsConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/vosp_contractsConfigVersion.cmake
    cmake/VospTesting.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vosp_contracts
)
