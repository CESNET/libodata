cmake_minimum_required(VERSION 3.0)
project (odata VERSION 0.0.1)

#project definition
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Wall -std=c++11 -pedantic -Wextra -Werror)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules" ${CMAKE_MODULE_PATH})

# check dependencies
find_package(CURL REQUIRED)
find_library(GLOG_LIBRARIES NAMES glog)
if(NOT GLOG_LIBRARIES)
    message(SEND_ERROR "glog library not found")
endif (NOT GLOG_LIBRARIES)
find_library(TINYXML2_LIBRARIES NAMES tinyxml2)
if(NOT TINYXML2_LIBRARIES)
    message(SEND_ERROR "tinyxml2 library not found")
endif (NOT TINYXML2_LIBRARIES)
find_package(Boost REQUIRED COMPONENTS filesystem thread serialization iostreams)
find_package(BerkeleyDB REQUIRED)


# library project
set(LIB_NAME odata)
add_library(${LIB_NAME} SHARED
    src/Archive.cpp src/Archive.h
    src/BerkeleyDBStorage.cpp src/BerkeleyDBStorage.h
    src/CachedStorage.cpp src/CachedStorage.h
    include/odata/Connection.h
    src/DataHubConnection.cpp include/odata/DataHubConnection.h
    src/DataHub.cpp include/odata/DataHub.h
    src/DataHubException.cpp include/odata/DataHubException.h
    src/Directory.cpp src/Directory.h
    src/File.cpp src/File.h
    src/FileSystemNode.cpp include/odata/FileSystemNode.h
    src/LRUCache.h
    src/Product.cpp src/Product.h
    src/ProductFile.h
    src/ProductPath.cpp src/ProductPath.h
    src/ProductPlaceHolder.cpp src/ProductPlaceHolder.h
    src/ProductStorage.cpp src/ProductStorage.h
    src/RemoteFile.cpp src/RemoteFile.h
    src/ScopeGuard.h
    src/SearchQuery.cpp src/SearchQuery.h
    src/SearchQueryBuilder.cpp src/SearchQueryBuilder.h
    src/TemporaryFile.cpp src/TemporaryFile.h
    src/XmlParser.cpp src/XmlParser.h
    )
target_include_directories(${LIB_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/odata>
    $<INSTALL_INTERFACE:include/odata>)
target_include_directories(${LIB_NAME} PUBLIC ${CURL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${BERKELEY_INCLUDE_DIRS})
target_link_libraries(${LIB_NAME} PUBLIC ${CURL_LIBRARIES})
target_link_libraries(${LIB_NAME} PUBLIC ${GLOG_LIBRARIES})
target_link_libraries(${LIB_NAME} PUBLIC ${TINYXML2_LIBRARIES})
target_link_libraries(${LIB_NAME} PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${LIB_NAME} PUBLIC ${BERKELEY_DB_LIBRARIES})

#install
include(CMakePackageConfigHelpers)
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Headers installation directory")
set(LIB_INSTALL_DIR lib/ CACHE PATH "Library installation directory")
set(CONFIG_INSTALL_DIR "lib/cmake/${PROJECT_NAME}" CACHE PATH "Package configuration directory")
set(PROJECT_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
set(VERSION_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
export(TARGETS ${LIB_NAME} FILE "${PROJECT_BINARY_DIR}/${TARGETS_EXPORT_NAME}.cmake")
export(PACKAGE ${PROJECT_NAME})
write_basic_package_version_file(
    ${VERSION_CONFIG} COMPATIBILITY SameMajorVersion)
configure_package_config_file(
    "Config.cmake.in"
    ${PROJECT_CONFIG}
    INSTALL_DESTINATION ${CONFIG_INSTALL_DIR})
install(TARGETS ${LIB_NAME} 
    EXPORT ${TARGETS_EXPORT_NAME}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
    INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR})
install(
    DIRECTORY include/odata DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)
install(
    FILES ${PROJECT_CONFIG} ${VERSION_CONFIG}
    DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT Devel)
install(
    EXPORT ${TARGETS_EXPORT_NAME} DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT Devel)

# test client
set(TEST_CLIENT odata-client)
add_executable(${TEST_CLIENT} src/main.cpp)
target_link_libraries(${TEST_CLIENT} ${LIB_NAME})

#test - enable only if gtest library is found
find_package(GTest)
if(GTEST_FOUND)
    enable_testing()
    set(TEST_BINARY runtest)
    add_executable(${TEST_BINARY}
        test/BerkeleyDBStorageTest.cpp
        test/DataHubTest.cpp
        test/DirectoryTest.cpp
        test/FileTest.cpp
        test/FileSystemNodeTest.cpp
        test/LRUCacheTest.cpp
        test/main.cpp
        test/MockConnection.cpp test/MockConnection.h
        test/MockStorage.h test/MockStorage.cpp
        test/ProductPathTest.cpp
        test/ProductTest.cpp
        test/RemoteFileTest.cpp
        test/SearchQueryTest.cpp
        test/SearchQueryBuilderTest.cpp
        test/Utils.cpp test/Utils.h
        test/XmlParserTest.cpp
        )
    add_test(tests ${TEST_BINARY})
    target_include_directories(${TEST_BINARY} PRIVATE ${GTEST_INCLUDE_DIRS} src)
    target_link_libraries(${TEST_BINARY} ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} pthread)
    target_link_libraries(${TEST_BINARY} ${LIB_NAME})
else(GTEST_FOUND)
    message(STATUS "GTEST package not found, tests are disabled")
endif(GTEST_FOUND)
