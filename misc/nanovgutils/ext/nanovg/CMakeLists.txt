project(nanovg)

find_package(OpenGL QUIET REQUIRED)


if(WIN32)
    add_definitions( "/W0 /D_CRT_SECURE_NO_WARNINGS /wd4267 /wd4244 /wd4005 /wd4996 /nologo /IGNORE:4006" )
endif()

set(nanovg_HEADERS  
    ${CMAKE_CURRENT_SOURCE_DIR}/fontstash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg.h    
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg_gl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg_gl_utils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg_internal.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovgapidefine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovgconfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/stb_truetype.h
)
                   
set(nanovg_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg_gl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nanovg_pick.cpp
)

add_library(nanovg ${nanovg_SOURCES} ${nanovg_HEADERS})
add_library(inviwo::nanovg ALIAS nanovg)

ivw_define_standard_definitions(nanovg nanovg)
ivw_suppress_compiler_warnings(nanovg)

target_include_directories(nanovg PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${OPENGL_INCLUDE_DIR}
)

# InViWo Macros
ivw_folder(nanovg ext)

target_link_libraries(nanovg PUBLIC 
    ${OPENGL_LIBRARIES} GLEW inviwo-module-opengl
)

#--------------------------------------------------------------------
# Make package (for other projects to find)
ivw_make_package(NanoVG nanovg)
ivw_default_install_targets(nanovg)

