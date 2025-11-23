ivw_module(FeatureLevelSetsGL)

set(HEADER_FILES
    include/inviwo/featurelevelsetsgl/featurelevelsetsglmodule.h
    include/inviwo/featurelevelsetsgl/featurelevelsetsglmoduledefine.h
    include/inviwo/featurelevelsetsgl/processors/featurelevelsetprocessorgl.h
    include/inviwo/featurelevelsetsgl/properties/traitproperty.h
    include/inviwo/featurelevelsetsgl/properties/implicitfunctiontraitproperty.h
    include/inviwo/featurelevelsetsgl/properties/pointtraitproperty.h
    include/inviwo/featurelevelsetsgl/properties/rangetraitproperty.h
    include/inviwo/featurelevelsetsgl/util/util.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/featurelevelsetsglmodule.cpp
    src/processors/featurelevelsetprocessorgl.cpp
    src/properties/traitproperty.cpp
    src/properties/implicitfunctiontraitproperty.cpp
    src/properties/pointtraitproperty.cpp
    src/properties/rangetraitproperty.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(SHADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/featurelevelsets.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/pointtrait.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/rangetrait.glsl
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/featurelevelsetsgl-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
