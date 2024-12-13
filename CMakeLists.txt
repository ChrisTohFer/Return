cmake_minimum_required(VERSION 3.20)

project(Return)

#set some global values
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#compiler/platform dependent values
if(MSVC_VERSION)
	add_compile_options(/permissive- /W4 /WX) #conformance, warnings, warnings as errors
endif()

#define each target, starting from lowest level dependency and ending with exe

#pseudo targets for grouping settings
add_library(GlobalSettings INTERFACE)
target_compile_definitions(GlobalSettings
	INTERFACE
	$<$<CONFIG:Debug,RelWithDebInfo>:DEBUG>
	$<$<CONFIG:Release,MinSizeRel>:RELEASE NDEBUG>
	$<$<CONFIG:Release,MinSizeRel,RelWithDebInfo>:OPTIMIZED>
	CONFIGURATION=$<CONFIG>
	"CONFIGURATION_STR=\"$<CONFIG>\""
	)

#thirdparty interface targets
add_library(Glfw INTERFACE)
target_include_directories(Glfw INTERFACE "third_party/glfw/include")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(Glfw INTERFACE "${CMAKE_SOURCE_DIR}/third_party/glfw/linux/libglfw3.a")
else()
	target_link_libraries(Glfw INTERFACE "${CMAKE_SOURCE_DIR}/third_party/glfw/win64/glfw3dll.lib")
endif()

#thirdparty source targets
file(GLOB_RECURSE glad_source "third_party/glad/*.h" "third_party/glad/*.c")
add_library(Glad STATIC ${glad_source})
target_include_directories(Glad PUBLIC "third_party/glad")

file(GLOB_RECURSE imgui_source "third_party/imgui/*.h" "third_party/imgui/*.cpp")
add_library(ImGui STATIC ${imgui_source})
target_include_directories(ImGui PUBLIC "third_party/imgui/include")
target_include_directories(ImGui PRIVATE "third_party/imgui/include/imgui")
target_link_libraries(ImGui PUBLIC Glfw)

#original targets
file(GLOB_RECURSE maths_source "source/maths/*.h" "source/maths/*.cpp")
add_library(Maths STATIC ${maths_source})
target_include_directories(Maths PUBLIC "source/maths/include")
target_include_directories(Maths PRIVATE "source/maths")
target_link_libraries(Maths PRIVATE GlobalSettings)

file(GLOB_RECURSE gfx_source "source/gfx/*.h" "source/gfx/*.cpp")
add_library(Gfx STATIC ${gfx_source})
target_include_directories(Gfx PUBLIC "source/gfx/include")
target_include_directories(Gfx PRIVATE "source/gfx/include/gfx")
target_include_directories(Gfx PRIVATE "source/gfx")
target_link_libraries(Gfx PRIVATE GlobalSettings)
target_link_libraries(Gfx PRIVATE Maths)
target_link_libraries(Gfx PRIVATE Glad)

file(GLOB_RECURSE return_source_files "source/Return/*.h" "source/Return/*.cpp")
add_executable(Return ${return_source_files})
target_link_libraries(Return PRIVATE GlobalSettings)
target_link_libraries(Return PUBLIC Glfw)
target_link_libraries(Return PUBLIC Glad)
target_link_libraries(Return PUBLIC ImGui)
target_link_libraries(Return PUBLIC Maths)
target_link_libraries(Return PUBLIC Gfx)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	set_property(TARGET Return PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_custom_command(TARGET Return POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/third_party/glfw/win64/glfw3.dll" $<TARGET_FILE_DIR:Return>/
	  )
endif()

##Add visual studio filters based on folders
#foreach(source IN LISTS source_list)
#	cmake_path(RELATIVE_PATH source OUTPUT_VARIABLE source_path)
#	cmake_path(REMOVE_FILENAME source_path)
#    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
#    source_group("${source_path_msvc}" FILES "${source}")
#endforeach()
