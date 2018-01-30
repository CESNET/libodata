cmake_minimum_required(VERSION 3.0)
project (odata-client)

# dependencies
set(EXTERNAL_PROJECTS_PREFIX ${CMAKE_BINARY_DIR}/external-projects)

include(ExternalProject)

ExternalProject_Add(Externalrestclient-cpp
     PREFIX "${EXTERNAL_PROJECTS_PREFIX}"
     GIT_REPOSITORY "https://github.com/mrtazz/restclient-cpp.git"
     GIT_TAG "master"
     CONFIGURE_COMMAND autoreconf --install COMMAND ./configure --prefix=<INSTALL_DIR>
     BUILD_COMMAND ${MAKE}
     BUILD_IN_SOURCE 1
     UPDATE_COMMAND ""
     )

set(REST_CLIENT_INCLUDE_DIR ${EXTERNAL_PROJECTS_PREFIX}/include/restclient-cpp)

message(STATUS "restclient-cpp include: ${REST_CLIENT_INCLUDE_DIR}")

#project definition
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${EXTERNAL_PROJECTS_PREFIX}/include)
add_library(libodata-client
    src/Connection.cpp src/Connection.h
    src/Product.cpp src/Product.h
    src/ProductPath.cpp src/ProductPath.h
    src/SearchQueryBuilder.cpp src/SearchQueryBuilder.h
    src/XmlParser.cpp src/XmlParser.h
    )
add_executable(${PROJECT_NAME} src/main.cpp)

add_library(restclient-cpp SHARED IMPORTED)
set_target_properties(restclient-cpp PROPERTIES IMPORTED_LOCATION ${EXTERNAL_PROJECTS_PREFIX}/lib/librestclient-cpp.a)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} tinyxml2)
target_link_libraries(${PROJECT_NAME} libodata-client)
target_link_libraries(${PROJECT_NAME} restclient-cpp)

add_dependencies(${PROJECT_NAME} Externalrestclient-cpp)

add_definitions(-Wall -std=c++14 -pedantic -Wextra)

#test
find_package(GTest REQUIRED)
enable_testing()
add_executable(runtest
    test/main.cpp 
    test/ProductPathTest.cpp
    test/SearchQueryBuilderTest.cpp
    test/XmlParserTest.cpp
    )
add_test(tests runtest)
include_directories(runtest ${GTEST_INCLUDE_DIRS} src)
target_link_libraries(runtest ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
target_link_libraries(runtest ${CURL_LIBRARIES})
target_link_libraries(runtest tinyxml2)
target_link_libraries(runtest libodata-client)
target_link_libraries(runtest restclient-cpp)
