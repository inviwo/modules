add_subdirectory(ext/nanovg)

ivw_module(NanoVGUtils)

set(HEADER_FILES
    include/inviwo/nanovgutils/nanovgutils.h
    include/inviwo/nanovgutils/nanovgutilsmodule.h
    include/inviwo/nanovgutils/nanovgutilsmoduledefine.h
    include/inviwo/nanovgutils/processors/nanovgpickingtestprocessor.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/nanovgutils.cpp
    src/nanovgutilsmodule.cpp
    src/processors/nanovgpickingtestprocessor.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(inviwo-module-nanovgutils PUBLIC nanovg)

target_link_libraries(inviwo-module-nanovgutils PUBLIC inviwo::nanovg)

ivw_make_package(InviwoNanoVGUtilsModule inviwo-module-nanovgutils)
