cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)
project(ChatApp)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})



include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(ChatApp ${SOURCES})
target_link_libraries(ChatApp ${wxWidgets_LIBRARIES})
target_include_directories(ChatApp PRIVATE ${wxWidgets_INCLUDE_DIRS})
