set(shared_source_files
    Datum.cpp
    StringTransform.cpp
    DateTimeParser.cpp
    File.cpp
    StackTrace.cpp
    base64.cpp
    misc.cpp
    thread_count.cpp
    MathUtils.cpp)
include_directories(${CMAKE_SOURCE_DIR})
if("${MAPD_EDITION_LOWER}" STREQUAL "ee")
  list(APPEND shared_source_files ee/Encryption.cpp)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/funcannotations.h DESTINATION ${CMAKE_BINARY_DIR}/Shared/)

add_library(Shared ${shared_source_files})
target_link_libraries(Shared Logger ${BLOSC_LIBRARIES} ${Boost_LIBRARIES})
if("${MAPD_EDITION_LOWER}" STREQUAL "ee")
  target_link_libraries(Shared ${OPENSSL_LIBRARIES})
endif()

# Required by ThriftClient.cpp
add_definitions("-DTHRIFT_PACKAGE_VERSION=\"${Thrift_VERSION}\"")
add_library(ThriftClient ThriftClient.cpp)
target_link_libraries(ThriftClient  ${Thrift_LIBRARIES} ${Boost_LIBRARIES})
