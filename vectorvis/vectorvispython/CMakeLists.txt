#--------------------------------------------------------------------
# Inviwo vectorvispython Module
ivw_module(VectorVisPython)

# Code uses type hinting generics in standard collections (new in 3.9)
find_package(Python3 3.9 REQUIRED)
# Check if netCDF4 python module exist
execute_process(COMMAND "${PYTHON_EXECUTABLE}" "-c"
             "try: import scipy; print(scipy.__version__);\nexcept: print('\tFailed to import scipy python package. Run pip install scipy')\n"
              RESULT_VARIABLE VECTORVISPYTHON_STATUS
              ERROR_VARIABLE VECTORVISPYTHON_ERROR 
              OUTPUT_STRIP_TRAILING_WHITESPACE
            )

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/vectorvispython/vectorvispythonmodule.h
    include/inviwo/vectorvispython/vectorvispythonmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/vectorvispythonmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python)
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/python/processors)
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})
