#include "window.h"

#include "dockspace.h"

#include "gfx/graphics_core.h"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/ImGuizmo.h"

#include <memory>


namespace re
{
    namespace
    {
        GLFWwindow* g_window = nullptr;
        InputManager* g_input_manager = nullptr;
        float g_aspect;
        bool g_first_update = true;
    }

    void error_callback(int /*error*/, const char* description)
    {
        printf("Error: %s\n", description);
    }
    void framebuffer_size_callback(GLFWwindow*, int width, int height)
    {
        gfx::resize_viewport(width, height);
        g_aspect = (float)width / (float)height;
    }

    //returns false if init failed for any reason
    bool window_init(const char* window_name, int width, int height)
    {
        //initiailize window
        g_aspect = (float)width / (float)height;
        glfwInit();
        glfwSetErrorCallback(error_callback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        auto window = glfwCreateWindow(
            width,
            height,
            window_name,
            nullptr,
            nullptr);

        if (!window)
        {
            return false;
        }
        glfwMakeContextCurrent(window);

        if (!gfx::init(glfwGetProcAddress, width, height))
        {
            glfwDestroyWindow(window);
            glfwTerminate();
            return false;
        }

        g_window = window;

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        auto& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        g_input_manager = new InputManager(*g_window);

        return true;
    }

    void window_shutdown()
    {
        g_first_update = true;

        assert(g_window != nullptr && g_input_manager);
        delete g_input_manager;
        g_input_manager = nullptr;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        gfx::shutdown();

        glfwDestroyWindow(g_window);
        glfwTerminate();
    }

    //return true if execution should continue, false if should exit
    bool window_update()
    {
        //clean up the previous frame
        //having this here is probbably a bit weird and restrictive but can change that later
        if (!g_first_update)
        {
            //imgui
            re::end_dockspace();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            //glfw
            glfwSwapBuffers(g_window);
            glfwPollEvents();
        }
        else
        {
            g_first_update = false;
        }

        if (glfwWindowShouldClose(g_window))
        {
            return false;
        }

        //imgui
        auto& io = ImGui::GetIO();
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

        //gfx
        gfx::clear(0.f, 0.f, 0.f, 0.f);
        re::begin_dockspace();

        //update input manager
        g_input_manager->update();

        return true;
    }

    float window_aspect()
    {
        return g_aspect;
    }

    const InputManager& window_input_manager()
    {
        assert(g_input_manager);
        return *g_input_manager;
    }
}