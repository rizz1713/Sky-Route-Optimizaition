cmake_minimum_required(VERSION 3.10)
project(FlightRouteOptimizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Find dependencies
find_package(Threads REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(flight_optimizer ${SOURCES})

# Link libraries
target_link_libraries(flight_optimizer PRIVATE Threads::Threads)

# Platform-specific settings
if(WIN32)
    target_link_libraries(flight_optimizer PRIVATE ws2_32)
endif()

# Set output directory
set_target_properties(flight_optimizer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)