cmake_minimum_required (VERSION 2.6)
include(ExternalProject)

set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wmissing-prototypes -Wdiv-by-zero -Wbad-function-cast -Wcast-align -Wcast-qual -Wfloat-equal -Wmissing-declarations -Wnested-externs -Wno-unknown-pragmas -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wswitch-enum -Wno-type-limits -fstack-protector -D_FORTIFY_SOURCE=2 -fPIE -fsanitize=address")
set(${CMAKE_C_FLAGS} "${CMAKE_C_FLAGS} -Wall -Wextra -Wmissing-prototypes -Wdiv-by-zero -Wbad-function-cast -Wcast-align -Wcast-qual -Wfloat-equal -Wmissing-declarations -Wnested-externs -Wno-unknown-pragmas -Wpointer-arith -Wredundant-decls -Wstrict-prototypes -Wswitch-enum -Wno-type-limits -fstack-protector -D_FORTIFY_SOURCE=2 -fPIE -fsanitize=address")

project(sfc)

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
include_directories(${LIBHYDROGEN_INCLUDE_DIR})

add_executable(${PROJECT_NAME} 
    src/sfc.cpp
)
target_link_libraries(${PROJECT_NAME} ${LIBHYDROGEN_LIBRARY})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)