cmake_minimum_required(VERSION 3.0)
project (odata VERSION 0.4.0)

#project definition
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-Wall -std=c++14 -pedantic -Wextra -Werror)
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
find_package(Boost REQUIRED COMPONENTS filesystem thread serialization iostreams program_options)
find_package(BerkeleyDB REQUIRED)


# generate version file
configure_file(src/Version.cpp.in Version.cpp)

# library project
set(LIB_NAME "odata")
set(LIB_COMPONENT_NAME "libodata")
set(DEVEL_COMPONENT_NAME "libodata-dev")
add_library(${LIB_NAME} SHARED
    src/BerkeleyDBStorage.cpp src/BerkeleyDBStorage.h
    src/CachedStorage.cpp src/CachedStorage.h
    src/Config.cpp src/Config.h
    include/odata/Connection.h
    src/DataHubConnection.cpp include/odata/DataHubConnection.h
    src/DataHub.cpp include/odata/DataHub.h
    src/DataHubException.cpp include/odata/DataHubException.h
    src/Directory.cpp src/Directory.h
    src/File.cpp src/File.h
    src/FileSystemNode.cpp include/odata/FileSystemNode.h
    src/LRUCache.h
    src/PathBuilder.cpp src/PathBuilder.h
    src/Product.cpp src/Product.h
    src/ProductAttribute.h
    src/ProductFile.h
    src/ProductPath.cpp src/ProductPath.h
    src/ProductPlaceHolder.cpp src/ProductPlaceHolder.h
    src/ProductStorage.h
    src/RemoteFile.cpp src/RemoteFile.h
    src/ScopeGuard.h
    src/SearchQuery.cpp src/SearchQuery.h
    src/SearchQueryBuilder.cpp src/SearchQueryBuilder.h
    src/SerializationHelpers.h
    src/TemporaryFile.cpp src/TemporaryFile.h
    ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp src/Version.h
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
    LIBRARY DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${LIB_COMPONENT_NAME}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT ${LIB_COMPONENT_NAME})
install(
    DIRECTORY include/odata DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT ${DEVEL_COMPONENT_NAME})
install(
    FILES ${PROJECT_CONFIG} ${VERSION_CONFIG}
    DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT ${DEVEL_COMPONENT_NAME})
install(
    EXPORT ${TARGETS_EXPORT_NAME} DESTINATION ${CONFIG_INSTALL_DIR} COMPONENT ${DEVEL_COMPONENT_NAME})

# test client
set(TEST_CLIENT odata-client)
add_executable(${TEST_CLIENT} src/main.cpp)
target_link_libraries(${TEST_CLIENT} ${LIB_NAME})

# fuse binary
find_package(FUSE)
if(FUSE_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26")
    set(FUSE_BINARY odatafs)
    add_executable(${FUSE_BINARY} src/fuse.cpp src/FuseAdapter.cpp src/FuseAdapter.h)
    target_link_libraries(${FUSE_BINARY} ${LIB_NAME})
    target_link_libraries(${FUSE_BINARY} ${FUSE_LIBRARIES})
    install(TARGETS ${FUSE_BINARY} RUNTIME DESTINATION bin COMPONENT ${FUSE_BINARY})
else(FUSE_FOUND)
    message(STATUS "FUSE package not found, skipping odatafs")
endif(FUSE_FOUND)

#test - enable only if gtest library is found
find_package(GTest)
if(GTEST_FOUND)
    enable_testing()
    set(TEST_BINARY runtest)
    add_executable(${TEST_BINARY}
        test/BerkeleyDBStorageTest.cpp
        test/CachedStorageTest.cpp
        test/ConfigTest.cpp
        test/DataHubTest.cpp
        test/DirectoryTest.cpp
        test/FileTest.cpp
        test/FileSystemNodeTest.cpp
        test/LRUCacheTest.cpp
        test/main.cpp
        test/MockConnection.cpp test/MockConnection.h
        test/MockStorage.h test/MockStorage.cpp
        test/PathBuilderTest.cpp
        test/ProductPathTest.cpp
        test/ProductPlaceHolderTest.cpp
        test/ProductTest.cpp
        test/RemoteFileTest.cpp
        test/SearchQueryTest.cpp
        test/SearchQueryBuilderTest.cpp
        test/TemporaryFileTest.cpp
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

#cpak definitions to make debian package
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jakub Valenta")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/jakub-valenta/libodata")
set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
set(
    CPACK_COMPONENT_LIBODATA_DESCRIPTION
    "Library providing access to data stored in Copernicus Data Hub through Open Search and OData APIs.")
set(
    CPACK_COMPONENT_LIBODATA-DEV_DESCRIPTION
    "Development package for libodata.")
set(
    CPACK_COMPONENT_ODATAFS_DESCRIPTION
    "Filesystem based on fuse providing access to data stored in Copernicus Data Hub through Open Search and OData APIs.")
set(CPACK_DEBIAN_LIBODATA-DEV_PACKAGE_DEPENDS "libboost-all-dev")
set(CPACK_DEBIAN_ODATAFS_PACKAGE_DEPENDS "odata-libodata")
set(CPACK_DEB_PACKAGE_COMPONENT ON)
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS ON)

include(CPack)
