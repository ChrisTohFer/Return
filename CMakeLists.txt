cmake_minimum_required(VERSION 3.20)

project(Return)

#set some global values
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#compiler/platform dependent values
if(MSVC_VERSION)
	add_compile_options(/permissive- /W4 /WX) #conformance, warnings, warnings as errors
endif()

#function definitions

function(create_library_target target)
	file(GLOB_RECURSE source "source/${target}/*.h" "source/${target}/*.cpp" "source/${target}/*.inl")
	add_library(${target} STATIC ${source})
	target_include_directories(${target} PUBLIC  "source/${target}/include")
	target_include_directories(${target} PRIVATE "source/${target}/include/${target}")
	target_include_directories(${target} PRIVATE "source/${target}/source")
	target_link_libraries(${target} PRIVATE GlobalSettings)
endfunction()

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
add_library(glfw INTERFACE)
target_include_directories(glfw INTERFACE "third_party/glfw/include")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(glfw INTERFACE "${CMAKE_SOURCE_DIR}/third_party/glfw/linux/libglfw3.a")
else()
	target_link_libraries(glfw INTERFACE "${CMAKE_SOURCE_DIR}/third_party/glfw/win64/glfw3dll.lib")
endif()

#thirdparty source targets
file(GLOB_RECURSE stb_image_source "third_party/stb_image/*.h" "third_party/stb_image/*.c")
add_library(stb_image STATIC ${stb_image_source})
target_include_directories(stb_image PUBLIC "third_party/stb_image")
target_include_directories(stb_image PRIVATE "third_party/stb_image/stb_image")

file(GLOB_RECURSE glad_source "third_party/glad/*.h" "third_party/glad/*.c")
add_library(glad STATIC ${glad_source})
target_include_directories(glad PUBLIC "third_party/glad")

file(GLOB_RECURSE imgui_source "third_party/imgui/*.h" "third_party/imgui/*.cpp")
add_library(imgui STATIC ${imgui_source})
target_include_directories(imgui PUBLIC "third_party/imgui/include")
target_include_directories(imgui PRIVATE "third_party/imgui/include/imgui")
target_link_libraries(imgui PUBLIC glfw)
if(MSVC_VERSION)
	#disable warnings for imgui as imguizmo build fails otherwise and I don't want to change third party code
	target_compile_options(imgui PRIVATE /permissive /W0 /WX-)
endif()

#original targets
create_library_target(maths)

create_library_target(file)

create_library_target(gfx)
target_link_libraries(gfx PRIVATE glad stb_image maths file)

create_library_target(editor_support)
target_link_libraries(editor_support PRIVATE imgui maths)

file(GLOB_RECURSE return_source_files "source/return/*.h" "source/return/*.cpp")
add_executable(return ${return_source_files})
target_link_libraries(return PRIVATE GlobalSettings)
target_link_libraries(return PRIVATE glfw glad imgui maths gfx file editor_support)

if(CMAKE_GENERATOR MATCHES "Visual Studio")
	#debugger working directory
	set_property(TARGET return PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
	
	#add todo as source
	target_sources(return PRIVATE "todo.txt")

	#group projects into folders
	set_property(GLOBAL PROPERTY USE_FOLDERS true)
	set_target_properties(glad PROPERTIES FOLDER "third_party")
	set_target_properties(imgui PROPERTIES FOLDER "third_party")
	set_target_properties(stb_image PROPERTIES FOLDER "third_party")

	set_target_properties(maths PROPERTIES FOLDER "intermediate")
	set_target_properties(gfx PROPERTIES FOLDER "intermediate")
	set_target_properties(file PROPERTIES FOLDER "intermediate")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_custom_command(TARGET return POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/third_party/glfw/win64/glfw3.dll" $<TARGET_FILE_DIR:return>/
	  )
endif()
