#include "task_manager.h"

#include "graphics_test.h"
#include "input_manager.h"

#include "gfx/graphics_manager.h"
#include "gfx/graphics_core.h"
#include "scene.h"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"

#include <iostream>

float g_aspect = 1.f;
void error_callback(int /*error*/, const char* description)
{
    printf("Error: %s\n", description);
}
void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    gfx::resize_viewport(width, height);
    g_aspect = (float)width / float(height);
}

int main()
{
    std::cout << "Running " << CONFIGURATION_STR << " build.\n";

    //initiailize window
    constexpr int winx = 700, winy = 700;
    g_aspect = (float)winx / float(winy);
    glfwInit();
    glfwSetErrorCallback(error_callback);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    auto window = glfwCreateWindow(winx, winy, "Test", nullptr, nullptr);

    if (!window)
    {
        return -1;
    }
    std::cout << "Created window" << std::endl;
    glfwMakeContextCurrent(window);

    if(!gfx::init(glfwGetProcAddress, winx, winy))
    {
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    gfx::GraphicsManager manager;
    re::GraphicsTestEditor editor;
    re::InputManager input_manager(*window);
    re::Scene scene(manager, input_manager);

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGuizmo::BeginFrame();
        ImGuiIO& io = ImGui::GetIO();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
        
        gfx::clear(0.f,0.f,0.f,1.f);

        input_manager.update();
        if(editor.edit())
        {
            editor.compile_assets(manager);
            scene.relink_assets();
        }
        scene.editor_ui();

        float time = (float)glfwGetTime();
        static float previous_time = time;
        scene.update_and_draw(time - previous_time, g_aspect);
        previous_time = time;

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    gfx::shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();
}