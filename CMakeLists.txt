cmake_minimum_required(VERSION 3.15)
project(vicon_bridge)

add_compile_options("-std=c++2a")

# ==============================================================================
# Calculate correct project version number
# ==============================================================================
# Read in the current version
file(READ version.txt PROJECT_VERSION)

# create array of version strings
string(REPLACE "." ";" VERSION_ARRAY ${PROJECT_VERSION})
# ------------------------------------------------------------------------------
# Major, Minor, Patch numbers
# ------------------------------------------------------------------------------
# set corresponding cmake version variables
list(POP_FRONT VERSION_ARRAY
	PROJECT_VERSION_MAJOR
	PROJECT_VERSION_MINOR
	PROJECT_VERSION_PATCH
)
# Remove the trailing newline from the patch number
string(REPLACE "\n" "" PROJECT_VERSION_PATCH ${PROJECT_VERSION_PATCH})

# ------------------------------------------------------------------------------
# Full version number
# ------------------------------------------------------------------------------
# Rebuild the project version including the current commit hash
string(JOIN "." PROJECT_VERSION
	${PROJECT_VERSION_MAJOR}
	${PROJECT_VERSION_MINOR}
	${PROJECT_VERSION_PATCH}
)
unset(VERSION_ARRAY) # delete aray of version strings
message(STATUS "VICON BRIDGE PROJECT VERSION ${PROJECT_VERSION}")

# ==============================================================================
# Setup the repository
# ==============================================================================
# This will perfom task such as setting up the git hooks for anyone who uses
# this project
execute_process(
	COMMAND           ./setup.sh
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

# ================================== Library config ==========================

# Vicon DataStream SDK library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# Vicon Bridge library
add_library(${PROJECT_NAME}
    SHARED
		src/vicon_bridge.cpp
)

target_link_directories(${PROJECT_NAME}
	PRIVATE
        /usr/local/lib/dls2
        /usr/lib/dls2
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
		include
    PRIVATE
		/usr/include/eigen3
		/usr/include/dls2
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
		dls_message_wrappers
		dls_estimator
		vicon_sdk
)

# ================================== Install =================================

install(
	DIRECTORY   include/
	COMPONENT   ${PROJECT_NAME}_dev
	DESTINATION /usr/include/dls2/state_estimator/${PROJECT_NAME}
)

install(
	TARGETS             ${PROJECT_NAME}
	LIBRARY DESTINATION /usr/lib/dls2/estimators
	COMPONENT           ${PROJECT_NAME}
)

# ================================== Packaging =================================

set(CPACK_PACKAGE_VERSION_MAJOR             ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR             ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH             ${PROJECT_VERSION_PATCH})
set(CPACK_DEBIAN_PACKAGE_VERSION            ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR                    "dlslab")
set(CPACK_PACKAGE_CONTACT                   "dlslab")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER         "dlslab")
set(CPACK_VERBATIM_VARIABLES                ON)
set(CPACK_GENERATOR                         "DEB")
set(CPACK_DEB_COMPONENT_INSTALL             ON)

set(CPACK_DEBIAN_VICON_BRIDGE_PACKAGE_NAME          "dls-vicon-bridge" CACHE INTERNAL "")
set(CPACK_DEBIAN_VICON_BRIDGE_FILE_NAME             "dls-vicon-bridge-${PROJECT_VERSION}.deb" CACHE INTERNAL "")  
set(CPACK_DEBIAN_VICON_BRIDGE_PACKAGE_DEPENDS       "dls2-runtime" CACHE INTERNAL "")

set(CPACK_DEBIAN_VICON_BRIDGE_DEV_PACKAGE_NAME      "dls-vicon-bridge-dev" CACHE INTERNAL "")
set(CPACK_DEBIAN_VICON_BRIDGE_DEV_FILE_NAME         "dls-vicon-bridge-dev-${PROJECT_VERSION}.deb" CACHE INTERNAL "")  
set(CPACK_DEBIAN_VICON_BRIDGE_DEV_PACKAGE_DEPENDS   "dls2-dev, dls-vicon-bridge" CACHE INTERNAL "")

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
list(REMOVE_ITEM CPACK_COMPONENTS_ALL "Unspecified")

include(CPack)