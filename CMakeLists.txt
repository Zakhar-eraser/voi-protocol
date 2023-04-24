cmake_minimum_required(VERSION 3.0.0)
project(proj_o4 VERSION 0.1.0)

include(CTest)
enable_testing()

# include_directories(${CMAKE_SOURCE_DIR}/include)
# link_directories(${CMAKE_SOURCE_DIR}/include)

# include_directories(${CMAKE_SOURCE_DIR})

# set(SOURCES ${CMAKE_SOURCE_DIR}/source/main.cpp ${CMAKE_SOURCE_DIR}/source/get_data.cpp ${CMAKE_SOURCE_DIR}/headers/get_data.h)

# set(SOURCE_LIB
#         source/get_data.cpp)


# set(HEADER_LIB
#         headers/get_data.h)


# set_target_properties(libproj PROPERTIES LINKER_LANGUAGE CXX)

file(GLOB SOURCES
    headers/*.h
    source/*.cpp
    include/common/*.h
    include/ardupilotmega/*.h
    )

include_directories(headers/ include/common/ include/ardupilotmega/)

add_executable(proj_o4 "test.cpp" ${SOURCES})
# add_library(libproj SHARED ${SOURCE_LIB} ${HEADER_LIB})


target_link_libraries(proj_o4 pthread)

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)
