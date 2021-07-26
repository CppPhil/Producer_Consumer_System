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
  include/cmd_args.h
  include/consumer.h
  include/producer.h
  include/ring_buffer.h
  include/sleep_thread.h
  include/thread.h)

set(
  SOURCES
  src/cmd_args.c
  src/consumer.c
  src/main.c
  src/producer.c
  src/ring_buffer.c
  src/sleep_thread.c
  src/thread.c)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/build/PTHREADS-BUILT/lib/pthreadVC3.lib
    Kernel32.lib)
endif()
