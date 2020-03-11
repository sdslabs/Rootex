file(GLOB OldGeneratedFiles ./generated/**)
file(GLOB_RECURSE SerializeSources **.cpp **.h)
file(GLOB_RECURSE SerializeHeaders **.h)
file(GLOB_RECURSE FlatBufferFiles **.fbs)

set_source_files_properties(${FlatBufferFiles} PROPERTIES
    GENERATED true
)

set_source_files_properties(${OldGeneratedFiles} PROPERTIES
    GENERATED true
)

set(ROOTEX_FLAT_BUFFERS_OUTPUT
    ${CMAKE_CURRENT_LIST_DIR}/generated/
CACHE INTERNAL "")

set(ROOTEX_INCLUDES
    ${ROOTEX_INCLUDES}
    ${CMAKE_CURRENT_LIST_DIR}
CACHE INTERNAL "")

set(ROOTEX_HEADERS
    ${ROOTEX_HEADERS}
    ${SerializeHeaders}
    ${FlatBufferFiles}
CACHE INTERNAL "")

set(ROOTEX_SOURCES
    ${ROOTEX_SOURCES}
    ${SerializeSources}
    ${FlatBufferFiles}
CACHE INTERNAL "")

set(ROOTEX_FLAT_BUFFERS
    ${FlatBufferFiles}
CACHE INTERNAL "")

add_library(SerializationGeneratedHeaders STATIC stub.cpp ${FlatBufferFiles})

add_custom_command(TARGET SerializationGeneratedHeaders
                   PRE_BUILD
                   COMMAND ${CMAKE_CURRENT_LIST_DIR}/../vendor/FlatBuffers/flatbuffers/flatc.exe --cpp --jsonschema -o ${ROOTEX_FLAT_BUFFERS_OUTPUT} ${ROOTEX_FLAT_BUFFERS})
