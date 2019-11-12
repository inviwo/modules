ivw_module(NanoVGUtils)

add_subdirectory(ext/nanovg)

set(HEADER_FILES
    include/inviwo/nanovgutils/nanovgcontext.h
    include/inviwo/nanovgutils/nanovgutils.h
    include/inviwo/nanovgutils/nanovgutilsmodule.h
    include/inviwo/nanovgutils/nanovgutilsmoduledefine.h
    include/inviwo/nanovgutils/processors/nanovgexampleprocessor.h
    include/inviwo/nanovgutils/processors/nanovgpickingexampleprocessor.h
    include/inviwo/nanovgutils/properties/nanovgfontproperty.h
)
ivw_group("Header Files" ${HEADER_FILES})

set(SOURCE_FILES
    src/nanovgcontext.cpp
    src/nanovgutils.cpp
    src/nanovgutilsmodule.cpp
    src/processors/nanovgexampleprocessor.cpp
    src/processors/nanovgpickingexampleprocessor.cpp
    src/properties/nanovgfontproperty.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})

ivw_make_package(InviwoNanoVGUtilsModule inviwo-module-nanovgutils)

target_link_libraries(inviwo-module-nanovgutils PUBLIC nanovg)

