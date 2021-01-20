# Inviwo module dependencies for current module
# List modules on the format "Inviwo<ModuleName>Module"
set(dependencies
    InviwoPython3Module 
)

# Mark the module as protected to prevent it from being reloaded
# when using runtime module reloading. 
#set(protected ON)

# By calling set(EnableByDefault ON) the module will be set to enabled 
# when initially being added to CMake. Default OFF.
set(EnableByDefault ON)