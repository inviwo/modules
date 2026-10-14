ivw_module(MoCapVis)

set(HEADER_FILES
    include/inviwo/mocapvis/mocapvismodule.h
    include/inviwo/mocapvis/mocapvismoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/mocapvismodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

set(PYTHON_FILES
    python/processors/DataFrameColumnMatcher.py
    python/processors/DataFrameL2norm.py
    python/processors/PlotConfiguration.py
)
ivw_group("Python Files" ${PYTHON_FILES})

ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES} ${PYTHON_FILES})

# Add python processors directory to install package
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)

# Add required Python modules
# set_property(GLOBAL APPEND PROPERTY IVW_PYTHON_ADDITIONAL_MODULES pandas pyarrow natsort)
