cmake_minimum_required(VERSION 3.8)

project(notify-sales)

set(CMAKE_CXX_STANDARD 11)


find_package(CURL REQUIRED)
find_package(Boost REQUIRED COMPONENTS date_time)
find_package(Cgicc REQUIRED)


message(STATUS "FOUND:  " ${CGICC_INCLUDE_DIR})
message(STATUS "FOUND:  " ${CGICC_LIBRARY})

IF (CURL_FOUND AND Boost_FOUND AND Cgicc_FOUND)
    include_directories(${CURL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CGICC_INCLUDE_DIR})
endif ()


set(SOURCE_FILES main.cpp)
add_executable(notify-sales ${SOURCE_FILES})

IF (CURL_FOUND AND Boost_FOUND)
    target_link_libraries(notify-sales ${CURL_LIBRARIES} ${Boost_LIBRARIES} ${CGICC_LIBRARY})
endif ()
