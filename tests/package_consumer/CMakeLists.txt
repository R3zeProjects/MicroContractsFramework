cmake_minimum_required(VERSION 3.25)
project(vosp_contracts_package_consumer LANGUAGES CXX)

find_package(vosp_contracts 0.9 REQUIRED CONFIG)

add_executable(vosp_contracts_package_consumer main.cpp)
target_link_libraries(vosp_contracts_package_consumer PRIVATE vosp::contracts)
target_compile_features(vosp_contracts_package_consumer PRIVATE cxx_std_23)
