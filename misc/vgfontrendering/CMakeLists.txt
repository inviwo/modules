#--------------------------------------------------------------------
# Inviwo VGFontRendering Module
ivw_module(VGFontRendering)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/vgfontrendering/vgfontrenderingmodule.h
    include/inviwo/vgfontrendering/vgfontrenderingmoduledefine.h
    include/inviwo/vgfontrendering/processors/vgtextoverlayprocessor.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/vgfontrenderingmodule.cpp
    src/processors/vgtextoverlayprocessor.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES})
