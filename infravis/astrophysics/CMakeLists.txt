ivw_module(AstroPhysics)

set(HEADER_FILES
    include/infravis/astrophysics/astrophysicsmodule.h
    include/infravis/astrophysics/astrophysicsmoduledefine.h
    include/infravis/astrophysics/processors/fitsvolumeraycaster.h
    include/infravis/astrophysics/shadercomponents/fitsnonlineardepthcomponent.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/astrophysicsmodule.cpp
    src/processors/fitsvolumeraycaster.cpp
    src/shadercomponents/fitsnonlineardepthcomponent.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(PYTHON_FILES
    python/astroviscommon.py
    python/ivwastrovis.py
    python/processors/AngleToVector2D.py
    python/processors/AxisSliceAnnotation.py
    python/processors/FitsDepthCorrection.py
    python/processors/FitsPolarizationSource.py
    python/processors/FitsVolumeSource.py
)
ivw_group("Python Files" ${PYTHON_FILES})

set(SHADER_FILES
	# Add shaders
)
ivw_group("Shader Files" ${SHADER_FILES})

set(TEST_FILES
    tests/unittests/astrophysics-unittest-main.cpp
)
ivw_add_unittest(${TEST_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${PYTHON_FILES})

ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)

set_property(GLOBAL APPEND PROPERTY IVW_PYTHON_ADDITIONAL_MODULES astropy)

# Add shader directory to install package
#ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
