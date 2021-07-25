cmake_minimum_required(VERSION 3.15)

enable_testing()

project(consumer_producer C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)


set(APP_NAME consumer_producer_app)

set(
  HEADERS
  include/main.h
)

set(
  SOURCES
  src/main.c
)

add_executable(${APP_NAME} ${HEADERS} ${SOURCES})

target_compile_definitions(${APP_NAME} PRIVATE TESTING)

target_include_directories(
  ${APP_NAME} 
  PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}/include)
