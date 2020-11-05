ivw_module(DataFrameClustering)

set(HEADER_FILES
    include/inviwo/dataframeclustering/dataframeclusteringmodule.h
    include/inviwo/dataframeclustering/dataframeclusteringmoduledefine.h
    include/inviwo/dataframeclustering/processors/dataframeclustering.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/dataframeclusteringmodule.cpp
    src/processors/dataframeclustering.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(TEST_FILES
    tests/unittests/dataframeclustering-unittest-main.cpp
)
# ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})
