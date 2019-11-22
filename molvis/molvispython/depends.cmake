# Inviwo module dependencies for current module
# List modules on the format "Inviwo<ModuleName>Module"
set(dependencies
	InviwoMolVisBaseModule
    InviwoPython3Module
)

if(PYTHONLIBS_FOUND)
    set(EnableByDefault ON)
endif()