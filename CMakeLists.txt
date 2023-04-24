cmake_minimum_required(VERSION 3.0.0)
project(proj_o4 VERSION 0.1.0)

set(PROJ_4_SRC lib/get_data.cpp)
set(ANTI-UAV_LINK_SRC lib/anti-uav_tcp_connector.c)

include_directories(headers/)

add_library(anti-uav_tcp_connector STATIC ${ANTI-UAV_LINK_SRC})
target_link_libraries(anti-uav_tcp_connector pthread)

add_executable(anti_uav_example programs/example.c)
target_link_libraries(anti_uav_example anti-uav_tcp_connector)

add_executable(proj_o4 programs/test.cpp ${PROJ_4_SRC})
target_link_libraries(proj_o4 pthread anti-uav_tcp_connector)