cmake_minimum_required(VERSION 3.25)

project(MicroContractsFramework VERSION 0.8.0 LANGUAGES CXX)

set(MICRO_CONTRACTS_FRAMEWORK_PRERELEASE "beta")

include(CTest)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(MCF_BUILD_EXAMPLES "Build MCF usage examples" ON)

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

if(BUILD_TESTING)
    add_library(MicroContractsFrameworkHeaderContracts OBJECT
        tests/header_contracts/configuration_header.cpp
        tests/header_contracts/error_header.cpp
        tests/header_contracts/logging_header.cpp
        tests/header_contracts/protocol_header.cpp
        tests/header_contracts/telemetry_header.cpp
        tests/header_contracts/transport_header.cpp)
    target_link_libraries(MicroContractsFrameworkHeaderContracts
        PRIVATE MicroContractsFramework)

    add_executable(MicroContractsFrameworkTests tests/contracts_tests.cpp)
    target_link_libraries(MicroContractsFrameworkTests PRIVATE MicroContractsFramework)
    add_test(NAME MicroContractsFramework.Tests COMMAND MicroContractsFrameworkTests)
endif()

if(MCF_BUILD_EXAMPLES)
    add_executable(MicroContractsFrameworkExample examples/basic.cpp)
    target_link_libraries(MicroContractsFrameworkExample PRIVATE MicroContractsFramework)
    add_executable(MicroContractsFrameworkContractsExample examples/contracts.cpp)
    target_link_libraries(MicroContractsFrameworkContractsExample PRIVATE MicroContractsFramework)
    if(BUILD_TESTING)
        add_test(NAME MicroContractsFramework.Example.Basic
            COMMAND MicroContractsFrameworkExample)
        add_test(NAME MicroContractsFramework.Example.Contracts
            COMMAND MicroContractsFrameworkContractsExample)
    endif()
endif()

install(TARGETS MicroContractsFramework EXPORT vosp_contractsTargets)
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
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vosp_contracts
)
