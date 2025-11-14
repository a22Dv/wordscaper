cmake_minimum_required(VERSION 3.22)
project(wordscapers_tests CXX)

set(CMAKE_CXX_STANDARD 20)

# Dependencies.
find_package(OpenCV REQUIRED)
include(FetchContent)
FetchContent_Declare(
  Tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy
  GIT_TAG v0.12.2
)
FetchContent_MakeAvailable(Tracy)

add_custom_target(
  data ALL
  COMMAND 
  ${CMAKE_COMMAND} -E copy_directory 
  "${CMAKE_SOURCE_DIR}/../data" 
  "${CMAKE_BINARY_DIR}/data"
)

file (GLOB_RECURSE SRC_MAIN "${CMAKE_SOURCE_DIR}/../src/*.cpp")
file (GLOB TESTS "${CMAKE_SOURCE_DIR}/src/*.cpp")

foreach (tsrc ${TESTS})
  get_filename_component(tname ${tsrc} NAME_WE)
  add_executable(${tname} ${tsrc} ${SRC_MAIN})
  target_include_directories(${tname} PRIVATE "${CMAKE_SOURCE_DIR}/../include" ${OpenCV_INCLUDE_DIRS})
  target_link_libraries(${tname} PRIVATE Tracy::TracyClient ${OpenCV_LIBS})
  target_compile_options(${tname} PRIVATE /EHsc)
endforeach()

