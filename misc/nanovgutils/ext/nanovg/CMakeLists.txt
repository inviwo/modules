project(nanovg)

find_package(OpenGL QUIET REQUIRED)
find_package(Freetype REQUIRED)

set(nanovg_HEADERS  
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/fontstash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/stb_truetype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg_gl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg_gl_utils.h
)
                   
set(nanovg_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/nanovg.c
)

add_library(nanovg STATIC ${nanovg_SOURCES} ${nanovg_HEADERS})

ivw_define_standard_definitions(nanovg nanovg)
ivw_suppress_compiler_warnings(nanovg)

target_include_directories(nanovg PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg/src/
    ${OPENGL_INCLUDE_DIR}
)

ivw_folder(nanovg ext)

target_compile_definitions(nanovg PUBLIC 
	NANOVG_GL3_IMPLEMENTATION
	FONS_USE_FREETYPE
)

if(NOT IVW_USE_EXTERNAL_GLEW)
    target_link_libraries(nanovg PUBLIC inviwo::GLEW)
else()
    find_package(GLEW REQUIRED)
    target_link_libraries(nanovg PUBLIC GLEW::GLEW)
endif()
target_link_libraries(nanovg PUBLIC 
    ${OPENGL_LIBRARIES}
    Freetype::Freetype
)

#--------------------------------------------------------------------
# Make package (for other projects to find)
ivw_make_package(NanoVG nanovg)
ivw_default_install_targets(nanovg)

