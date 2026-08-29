cmake_minimum_required(VERSION 3.25)
project(vosp_contracts_package_consumer LANGUAGES CXX)

include(CTest)

find_package(vosp_contracts 0.12 REQUIRED CONFIG)

add_executable(vosp_contracts_package_consumer main.cpp)
target_link_libraries(vosp_contracts_package_consumer PRIVATE vosp::contracts vosp::testing)

vosp_add_compile_fail_test(
    NAME vosp_contracts_package_consumer.compile_fail
    SOURCES invalid_contract.cpp
    LINK_LIBRARIES vosp::contracts)
target_compile_features(vosp_contracts_package_consumer PRIVATE cxx_std_23)
