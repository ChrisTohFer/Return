cmake_minimum_required(VERSION 3.20)

project(Return)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#define each target, starting from lowest level dependency and ending with exe

add_library(Glfw INTERFACE)
target_include_directories(Glfw INTERFACE "third_party/glfw/include")
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(Glfw INTERFACE "${CMAKE_SOURCE_DIR}/third_party/glfw/linux/libglfw3.a")
else()
	target_link_libraries(Glfw INTERFACE "${CMAKE_SOURCE_DIR}/third_party/glfw/win64/glfw3dll.lib")
endif()

file(GLOB_RECURSE glad_source "third_party/glad/*.h" "third_party/glad/*.c")
add_library(Glad STATIC ${glad_source})
target_include_directories(Glad PUBLIC "third_party/glad")

file(GLOB_RECURSE imgui_source "third_party/imgui/*.h" "third_party/imgui/*.cpp")
add_library(ImGui STATIC ${imgui_source})
target_include_directories(ImGui PUBLIC "third_party/imgui/include")
target_include_directories(ImGui PRIVATE "third_party/imgui/include/imgui")
target_link_libraries(ImGui PUBLIC Glfw)

file(GLOB_RECURSE return_source_files "source/Return/*.h" "source/Return/*.cpp")
add_executable(Return ${return_source_files})
target_link_libraries(Return PUBLIC Glfw)
target_link_libraries(Return PUBLIC Glad)
target_link_libraries(Return PUBLIC ImGui)

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
