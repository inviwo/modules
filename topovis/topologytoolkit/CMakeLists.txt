# Boost is required by TTK
find_package(Boost REQUIRED)

#--------------------------------------------------------------------
# Build TTK
add_subdirectory(ext/ttk)

#--------------------------------------------------------------------
# Inviwo TopologyToolKit Module
ivw_module(TopologyToolKit)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES
    include/inviwo/topologytoolkit/datastructures/contourtreedata.h
    include/inviwo/topologytoolkit/datastructures/morsesmalecomplexdata.h
    include/inviwo/topologytoolkit/datastructures/triangulationdata.h
    include/inviwo/topologytoolkit/ports/contourtreeport.h
    include/inviwo/topologytoolkit/ports/morsesmalecomplexport.h
    include/inviwo/topologytoolkit/ports/persistencediagramport.h
    include/inviwo/topologytoolkit/ports/triangulationdataport.h
    include/inviwo/topologytoolkit/processors/contourtree.h
    include/inviwo/topologytoolkit/processors/contourtreetodataframe.h
    include/inviwo/topologytoolkit/processors/contourtreetomesh.h
    include/inviwo/topologytoolkit/processors/meshtotriangulation.h
    include/inviwo/topologytoolkit/processors/morsesmalecomplex.h
    include/inviwo/topologytoolkit/processors/morsesmalecomplextomesh.h
    include/inviwo/topologytoolkit/processors/persistencecurve.h
    include/inviwo/topologytoolkit/processors/persistencediagram.h
    include/inviwo/topologytoolkit/processors/separatrixrefiner.h
    include/inviwo/topologytoolkit/processors/topologicalsimplification.h
    include/inviwo/topologytoolkit/processors/triangulationtomesh.h
    include/inviwo/topologytoolkit/processors/triangulationtovolume.h
    include/inviwo/topologytoolkit/processors/ttktestprocessor.h
    include/inviwo/topologytoolkit/processors/volumetotriangulation.h
    include/inviwo/topologytoolkit/properties/topologycolorsproperty.h
    include/inviwo/topologytoolkit/topologytoolkitmodule.h
    include/inviwo/topologytoolkit/topologytoolkitmoduledefine.h
    include/inviwo/topologytoolkit/utils/ttkexception.h
    include/inviwo/topologytoolkit/utils/ttkutils.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    src/datastructures/contourtreedata.cpp
    src/datastructures/morsesmalecomplexdata.cpp
    src/datastructures/triangulationdata.cpp
    src/ports/contourtreeport.cpp
    src/ports/morsesmalecomplexport.cpp
    src/ports/persistencediagramport.cpp
    src/ports/triangulationdataport.cpp
    src/processors/contourtree.cpp
    src/processors/contourtreetodataframe.cpp
    src/processors/contourtreetomesh.cpp
    src/processors/meshtotriangulation.cpp
    src/processors/morsesmalecomplex.cpp
    src/processors/morsesmalecomplextomesh.cpp
    src/processors/persistencecurve.cpp
    src/processors/persistencediagram.cpp
    src/processors/separatrixrefiner.cpp
    src/processors/topologicalsimplification.cpp
    src/processors/triangulationtomesh.cpp
    src/processors/triangulationtovolume.cpp
    src/processors/ttktestprocessor.cpp
    src/processors/volumetotriangulation.cpp
    src/properties/topologycolorsproperty.cpp
    src/topologytoolkitmodule.cpp
    src/utils/ttkexception.cpp
    src/utils/ttkutils.cpp
)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add Unittests
set(TEST_FILES
	#${CMAKE_CURRENT_SOURCE_DIR}/tests/unittests/ttk-test.cpp
)
ivw_add_unittest(${TEST_FILES})

#--------------------------------------------------------------------
# Create module
ivw_create_module(NO_PCH ${SOURCE_FILES} ${HEADER_FILES})

set_target_properties(inviwo-module-topologytoolkit PROPERTIES COTIRE_PREFIX_HEADER_PUBLIC_IGNORE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ext/ttk")
ivw_compile_optimize_on_target(inviwo-module-topologytoolkit)

target_include_directories(inviwo-module-topologytoolkit PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ext/ttk>
)
target_link_libraries(inviwo-module-topologytoolkit PUBLIC baseAll)

ivw_register_license_file(NAME "Topology ToolKit (TTK)" VERSION 0.9.4 MODULE TopologyToolKit
    URL https://topology-tool-kit.github.io/
    TYPE "TTK, Copyright (c) 2016, CNRS & UPMC"
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/ext/ttk/ttk/LICENSE
)

ivw_register_license_file(NAME "boost C++ Libraries)" 
	VERSION ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION} MODULE TopologyToolKit
    URL https://www.boost.org
    TYPE "Boost Software License - Version 1.0"
)
