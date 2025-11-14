cmake_minimum_required(VERSION 3.22)

set(PNAME wordscaper)
set(PLANG CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(${PNAME} ${PLANG})

# Dependencies.
find_package(OpenCV REQUIRED)
include(FetchContent)
FetchContent_Declare(
  Tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy
  GIT_TAG v0.12.2
)
FetchContent_MakeAvailable(Tracy)

file(GLOB SRC_FILES 
"${CMAKE_SOURCE_DIR}/src/*.cpp" 
"${CMAKE_SOURCE_DIR}/app/main.cpp"
)
set(SOURCES ${SRC_FILES})
set(INCLUDES "${CMAKE_SOURCE_DIR}/include" ${OpenCV_INCLUDE_DIRS})

set(LINK_LIBRARIES Tracy::TracyClient ${OpenCV_LIBS})
set(COMPILE_OPTIONS /EHsc)
# set(COMPILE_DEFINITIONS <DEFS>)

set(ADD_MSVC_COMPILE_OPTS /W4 /WX)
set(ADD_GCC_CLANG_COMPILE_OPTS -Wall -Wextra -Wpedantic -Werror)

add_executable(${PNAME})
add_custom_command(
  TARGET ${PNAME} POST_BUILD
  COMMAND 
  ${CMAKE_COMMAND} -E copy_directory 
  "${CMAKE_SOURCE_DIR}/data" 
  "${CMAKE_BINARY_DIR}/data"
)

target_sources(${PNAME} PRIVATE ${SOURCES})
target_include_directories(${PNAME} PRIVATE ${INCLUDES})
target_link_libraries(${PNAME} PRIVATE ${LINK_LIBRARIES})
target_compile_options(${PNAME} PRIVATE ${COMPILE_OPTIONS})
# target_compile_definitions(${PNAME} PRIVATE ${COMPILE_DEFINITIONS})

if (MSVC) 
	target_compile_options(${PNAME} PRIVATE ${ADD_MSVC_COMPILE_OPTS})
else() 
	target_compile_options(${PNAME} PRIVATE ${ADD_GCC_CLANG_COMPILE_OPTS})
endif()
