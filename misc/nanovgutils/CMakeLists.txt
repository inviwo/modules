ivw_module(NanoVGUtils)

add_subdirectory(ext)

set(HEADER_FILES
    include/inviwo/nanovgutils/nanovgcontext.h
    include/inviwo/nanovgutils/nanovgutilsmodule.h
    include/inviwo/nanovgutils/nanovgutilsmoduledefine.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/nanovgcontext.cpp
    src/nanovgutilsmodule.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

ivw_make_package(InviwoNanoVGUtilsModule inviwo-module-nanovgutils)

target_link_libraries(inviwo-module-nanovgutils PUBLIC nanovg)

