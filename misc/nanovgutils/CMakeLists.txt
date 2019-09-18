ivw_module(NanoVGUtils)

set(HEADER_FILES
    include/inviwo/nanovgutils/nanovgutils.h
    include/inviwo/nanovgutils/nanovgutilsmodule.h
    include/inviwo/nanovgutils/nanovgutilsmoduledefine.h
	include/inviwo/nanovgutils/datastructures/nanovgcontext.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/nanovgutils.cpp
    src/nanovgutilsmodule.cpp
	src/datastructures/nanovgcontext.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

ivw_make_package(InviwoNanoVGUtilsModule inviwo-module-nanovgutils)

target_include_directories(inviwo-module-nanovgutils PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ext/nanovg/src)

add_definitions(-DNANOVG_GL3_IMPLEMENTATION)
