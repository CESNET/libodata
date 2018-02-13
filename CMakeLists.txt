cmake_minimum_required(VERSION 3.0)
project (odata-client)

#project definition
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${EXTERNAL_PROJECTS_PREFIX}/include)
add_library(libodata-client
    src/Connection.cpp src/Connection.h
    src/Directory.cpp src/Directory.h
    src/FileSystemNode.cpp src/FileSystemNode.h
    src/Product.cpp src/Product.h
    src/ProductPath.cpp src/ProductPath.h
    src/SearchQuery.cpp src/SearchQuery.h
    src/SearchQueryBuilder.cpp src/SearchQueryBuilder.h
    src/XmlParser.cpp src/XmlParser.h
    )
add_executable(${PROJECT_NAME} src/main.cpp)

find_package(CURL REQUIRED)
target_link_libraries(${PROJECT_NAME} ${CURL_LIBRARIES})
target_link_libraries(${PROJECT_NAME} tinyxml2)
target_link_libraries(${PROJECT_NAME} libodata-client)

add_definitions(-Wall -std=c++14 -pedantic -Wextra)

#test
find_package(GTest REQUIRED)
enable_testing()
add_executable(runtest
    test/DirectoryTest.cpp
    test/main.cpp 
    test/ProductPathTest.cpp
    test/SearchQueryTest.cpp
    test/SearchQueryBuilderTest.cpp
    test/XmlParserTest.cpp
    )
add_test(tests runtest)
include_directories(runtest ${GTEST_INCLUDE_DIRS} src)
target_link_libraries(runtest ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
target_link_libraries(runtest tinyxml2)
target_link_libraries(runtest libodata-client)
