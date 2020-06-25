cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0074 NEW)
set (CMAKE_CXX_STANDARD 11)


project (sharedmemory)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Boost dependency
# Include BoostLib module
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_DEBUG_LIBS ON)
set(Boost_USE_RELEASE_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF) 


if (NOT DEFINED  ENV{BOOST_ROOT})
	message(WARNING "BOOST_ROOT not found.")
	if (CMAKE_COMPILER_IS_GNUCXX)
		set(BOOST_ROOT /usr/local/boost)
	else()
		set(BOOST_ROOT D:/boost/boost_1_69_0)
  endif()
else()
  message("BOOST_ROOT found.")  
endif()

find_package(Boost 1.69.0 COMPONENTS system program_options date_time regex thread serialization)
if(Boost_FOUND)
	message("FOUND: ${Boost_INCLUDE_DIRS} | ${Boost_LIBRARIES}")
else()
	message("BOOST NOT FOUND.")
endif()

include_directories(${CMAKE_JS_INC};${Boost_INCLUDE_DIRS})
file(GLOB SOURCE_FILES "*.cc" "*.h")
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC} ${BOOST_LIBRARY_DIR})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB};${Boost_LIBRARIES})

# Include N-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

# define NPI_VERSION
add_definitions(-DNAPI_VERSION=3)

set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND "node.exe")
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "./test.js")
set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

