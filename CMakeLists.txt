cmake_minimum_required(VERSION 3.29.2)

set(PROGRAM_VERSION "MoveProject 1.0.0")
set(PROGRAM_VERSION_NUMBER 1.0.0.0)

project("MoveProject" VERSION ${PROGRAM_VERSION_NUMBER} LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform Detection
if (WIN32)
    message(STATUS "[MOVE_PROJECT] Platform = Windows")
else()
    message(FATAL_ERROR "[MOVE_PROJECT] Unsupported platform. Only Windows is supported.")
endif()

# Build Type Detection
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(IS_DEBUG TRUE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(IS_RELEASE TRUE)
else()
    message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE: '${CMAKE_BUILD_TYPE}'! Must be Debug, Release, or RelWithDebInfo.")
endif()

# Runtime lib handling for MSVC
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

# Directory Setup
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Source Files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*/*.cpp"
)

# Executable
add_executable(MoveProject ${SOURCE_FILES})

# Enable exception handling for MSVC
if (MSVC)
    target_compile_options(MoveProject PRIVATE /EHsc)
endif()

set_target_properties(MoveProject PROPERTIES OUTPUT_NAME "MoveProject")
target_compile_features(MoveProject PRIVATE cxx_std_20)

# Includes
file(GLOB_RECURSE HEADERS
	configure_depends
	"${CMAKE_SOURCE_DIR}/include/*.hpp"
)
target_sources(MoveProject PRIVATE ${HEADERS})
target_include_directories(MoveProject PRIVATE
	"${INCLUDE_DIR}"
)

# Preprocessor Defines
target_compile_definitions(MoveProject PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)

# Hide console in release mode
if(IS_RELEASE)
    set_target_properties(MoveProject PROPERTIES WIN32_EXECUTABLE TRUE)
    if (MSVC)
        set_target_properties(MoveProject PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup")
    endif()
endif()

# Installation
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS MoveProject DESTINATION ${CMAKE_INSTALL_BINDIR})

# Package
include(CPack)
