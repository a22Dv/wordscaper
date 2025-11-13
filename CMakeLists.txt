cmake_minimum_required(VERSION 3.22)

set(PNAME <NAME>)
set(PLANG <C/CXX>)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(${PNAME} ${PLANG})

set(SOURCES <SOURCE_FILES>)
set(INCLUDES <INCLUDE_DIRS>)

set(LINK_LIBRARIES <LIBS>)
set(COMPILE_OPTIONS <OPTS>)
set(COMPILE_DEFINITIONS <DEFS>)

set(ADD_MSVC_COMPILE_OPTS /W4 /WX)
set(ADD_GCC_CLANG_COMPILE_OPTS -Wall -Wextra -Wpedantic -Werror)

add_executable(${PNAME})

target_sources(${PNAME} PRIVATE ${SOURCES})
target_include_directories(${PNAME} PRIVATE ${INCLUDES})
target_link_libraries(${PNAME} PRIVATE ${LINK_LIBRARIES})
target_compile_options(${PNAME} PRIVATE ${COMPILE_OPTIONS})
target_compile_definitions(${PNAME} PRIVATE ${COMPILE_DEFINITIONS})

if (MSVC) 
	target_compile_options(${PNAME} PRIVATE ${ADD_MSVC_COMPILE_OPTS})
else() 
	target_compile_options(${PNAME} PRIVATE ${ADD_GCC_CLANG_COMPILE_OPTS})
endif()
