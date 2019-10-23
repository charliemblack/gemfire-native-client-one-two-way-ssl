cmake_minimum_required(VERSION 3.10)

project(SampleApp)

set(CMAKE_CXX_STANDARD 11)

include_directories( $ENV{GEMFIRE_NATIVE_HOME}/include )
link_directories( $ENV{GEMFIRE_NATIVE_HOME}/lib )
add_executable(SampleApp main.cpp UsernamePassword.cpp UsernamePassword.h)

target_link_libraries (SampleApp pivotal-gemfire cryptoImpl)
