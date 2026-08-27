cmake_minimum_required(VERSION 3.25)

project(VospEcosystemPackageConsumer LANGUAGES CXX)

include(CTest)

find_package(vosp_contracts 0.8 REQUIRED CONFIG)
find_package(mprotocol 0.2 REQUIRED CONFIG)
find_package(vosp 0.6 REQUIRED CONFIG)
find_package(mpf 0.3 REQUIRED CONFIG)
find_package(mtf 0.1.1 REQUIRED CONFIG)
find_package(mconfig 0.1 REQUIRED CONFIG)
find_package(mrf 0.1 REQUIRED CONFIG)
find_package(mwf 0.1 REQUIRED CONFIG)
find_package(msf 0.1 REQUIRED CONFIG)

add_executable(vosp_ecosystem_package_consumer main.cpp)
target_compile_features(vosp_ecosystem_package_consumer PRIVATE cxx_std_23)
target_link_libraries(vosp_ecosystem_package_consumer PRIVATE
    vosp::contracts
    vosp::protocol
    vosp::transport
    vosp::vosp
    vosp::persistence
    vosp::telemetry
    vosp::configuration
    vosp::resilience
    vosp::workflow
    vosp::service)

add_test(NAME VospEcosystem.PackageConsumer
    COMMAND vosp_ecosystem_package_consumer)
