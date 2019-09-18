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

ivw_register_license_file(NAME "Roboto Typeface" VERSION 2.0 MODULE VGFontRendering
    URL https://fonts.google.com/specimen/Roboto
    TYPE "Apache License"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/fonts/LICENSE-2.0.txt
)

ivw_register_license_file(NAME "Open Sans Typeface" VERSION 2.0 MODULE VGFontRendering
    URL https://fonts.google.com/specimen/Open+Sans
    TYPE "Apache License"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/fonts/LICENSE-2.0.txt
)
