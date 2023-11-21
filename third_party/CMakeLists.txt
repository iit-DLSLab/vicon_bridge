cmake_minimum_required(VERSION 3.15)
project(vicon_sdk)

add_compile_options("-std=c++2a")

# ================================== Library config ==========================
add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib/libViconDataStreamSDK_CPP.so)

# Vicon Test
add_executable(vicon_test ViconDataStreamSDK_CPPTest.cpp)

target_link_libraries(vicon_test PRIVATE ${PROJECT_NAME})