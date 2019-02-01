cmake_minimum_required (VERSION 2.6)
include(ExternalProject)

project(sfc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wmissing-prototypes -Wdiv-by-zero -Wbad-function-cast -Wcast-align -Wcast-qual -Wfloat-equal -Wmissing-declarations -Wnested-externs -Wno-unknown-pragmas -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wswitch-enum -Wno-type-limits -fstack-protector -D_FORTIFY_SOURCE=2 -fPIE -fno-omit-frame-pointer -fsanitize=address")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wmissing-prototypes -Wdiv-by-zero -Wbad-function-cast -Wcast-align -Wcast-qual -Wfloat-equal -Wmissing-declarations -Wnested-externs -Wno-unknown-pragmas -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wswitch-enum -Wno-type-limits -fstack-protector -D_FORTIFY_SOURCE=2 -fPIE -fno-omit-frame-pointer -fsanitize=address")

ExternalProject_Add(libhydrogen
  GIT_REPOSITORY    https://github.com/jedisct1/libhydrogen.git
  GIT_TAG           master
  UPDATE_DISCONNECTED true
  SOURCE_SUBDIR     .
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE   true
  BUILD_COMMAND     make
  INSTALL_COMMAND   ""
)
ExternalProject_Get_property(libhydrogen SOURCE_DIR)
SET(LIBHYDROGEN_SOURCE_DIR ${SOURCE_DIR})
SET(LIBHYDROGEN_INCLUDE_DIR ${LIBHYDROGEN_SOURCE_DIR})
SET(LIBHYDROGEN_LIBRARY ${LIBHYDROGEN_SOURCE_DIR}/libhydrogen.a)

ExternalProject_Add(CLI11
  URL               https://github.com/CLIUtils/CLI11/releases/download/v1.7.0/CLI11.hpp
  DOWNLOAD_NO_EXTRACT true
  UPDATE_DISCONNECTED true
  CONFIGURE_COMMAND ""
  #BUILD_IN_SOURCE   true
  BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
)
ExternalProject_Get_property(CLI11 DOWNLOADED_FILE)
GET_FILENAME_COMPONENT(CLI11_INCLUDE_DIR ${DOWNLOADED_FILE} DIRECTORY)

ExternalProject_Add(nng
  GIT_REPOSITORY    https://github.com/nanomsg/nng.git
  GIT_TAG           master
  UPDATE_DISCONNECTED true
  CONFIGURE_COMMAND mkdir -p build && cd build && cmake ..
  BUILD_IN_SOURCE   true
  BUILD_COMMAND     cd build && make
  INSTALL_COMMAND   ""
)
ExternalProject_Get_property(nng SOURCE_DIR)
SET(NNG_SOURCE_DIR ${SOURCE_DIR})
SET(NNG_INCLUDE_DIR ${NNG_SOURCE_DIR}/include)
SET(NNG_LIBRARY ${NNG_SOURCE_DIR}/build/libnng.a)

message(STATUS "NNG_INCLUDE_DIR=${NNG_INCLUDE_DIR}")

add_executable(${PROJECT_NAME} 
    src/sfc.cpp
    src/cli.cpp
)
add_dependencies(${PROJECT_NAME} libhydrogen CLI11 nng)
target_include_directories(${PROJECT_NAME} PUBLIC ${CLI11_INCLUDE_DIR} ${LIBHYDROGEN_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LIBHYDROGEN_LIBRARY} ${NNG_LIBRARY})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

add_executable(nng-toy 
    src/nng-toy.cpp
)
set_property(TARGET nng-toy PROPERTY CXX_STANDARD 17)
target_link_libraries(nng-toy ${NNG_LIBRARY})
target_include_directories(nng-toy PUBLIC ${NNG_INCLUDE_DIR})
