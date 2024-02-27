# Inviwo vcpkg ports

## jsoncpp
Avoid creating multiple `JsonCpp::JsonCpp` library targets
+ added `jsoncpp-target.patch`
	Modifications in `jsoncpp-namespaced-targets.cmake`:
	```cmake
-elseif (TARGET jsoncpp_lib)
+elseif (TARGET jsoncpp_lib AND NOT TARGET JsonCpp::JsonCpp)
	```
+ apply patch in portfile.cmake

## vtk
Fix problem with multiple find_package(VTK) calls which will result in trying to add a global property multiple times, see https://github.com/microsoft/vcpkg/issues/35223
+ added `findpegtl.patch`
	Modifications in `find_pegtl.cmake`:
	```cmake
-if(TARGET taocpp::pegtl)
-    message(STATUS "Searching for PEGTL - found target taocpp::pegtl")
-    set_target_properties(taocpp::pegtl PROPERTIES IMPORTED_GLOBAL TRUE)
-    if(NOT TARGET PEGTL::PEGTL)
+if(TARGET taocpp::pegtl)
+    message(STATUS "Searching for PEGTL - found target taocpp::pegtl")
+    get_target_property(TARGET_IMPORTED_GLOBAL taocpp::pegtl IMPORTED_GLOBAL)
+    if(NOT TARGET_IMPORTED_GLOBAL)
+        set_target_properties(taocpp::pegtl PROPERTIES IMPORTED_GLOBAL TRUE)
+    endif()
+    if(NOT TARGET PEGTL::PEGTL)

	``` 
+ apply patch in portfile.cmake
