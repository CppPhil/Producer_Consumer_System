cmake_minimum_required(VERSION 3.15)

enable_testing()

project(consumer_producer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(APP_NAME consumer_producer_app)

set(
  HEADERS
  include/byte.h
  include/ring_buffer.h)

set(
  SOURCES
  src/main.c
  src/ring_buffer.c)

add_executable(${APP_NAME} ${HEADERS} ${SOURCES})

target_compile_definitions(${APP_NAME} PRIVATE TESTING)

target_include_directories(
  ${APP_NAME} 
  PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (WIN32)
  target_include_directories(
    ${APP_NAME} 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/build/PTHREADS-BUILT/include)

  target_link_libraries(
    ${APP_NAME}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/build/PTHREADS-BUILT/lib/pthreadVC3.lib)
endif()
