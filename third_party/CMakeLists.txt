cmake_minimum_required(VERSION 3.15)
project(vicon_sdk)

add_compile_options("-std=c++2a")

# ================================== Library config ==========================
add_library(vicon_sdk INTERFACE)

target_include_directories(vicon_sdk INTERFACE .)

target_link_libraries(vicon_sdk
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/libViconDataStreamSDK_CPP.so
)

add_executable(vicon_test ViconDataStreamSDK_CPPTest.cpp)
target_link_libraries(vicon_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libViconDataStreamSDK_CPP.so)

# ================================== Install =================================

install(
	DIRECTORY   	.
	COMPONENT   	${PROJECT_NAME}_dev
	DESTINATION 	/usr/include/dls2/state_estimator/vicon_bridge/dls/state_estimator
	FILES_MATCHING	PATTERN "*.h"
)

install(
	DIRECTORY   	.
	COMPONENT   	${PROJECT_NAME}
	DESTINATION 	/usr/lib/dls2/estimators
	FILES_MATCHING	PATTERN "*.so"
)