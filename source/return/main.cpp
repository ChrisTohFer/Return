#include "task_manager.h"

#include "graphics_test.h"
#include "gfx/graphics_manager.h"
#include "scene.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>

float g_aspect = 1.f;
void error_callback(int /*error*/, const char* description)
{
    printf("Error: %s\n", description);
}
void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
    g_aspect = (float)width / float(height);
}

int main()
{
    std::cout << "Running " << CONFIGURATION_STR << " build.\n";

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

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }
    std::cout << "gladLoadGLLoader successfull" << std::endl;
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    glViewport(0, 0, winx, winy);

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.f, 0.f, 0.f, 255.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static re::GraphicsTestEditor editor;
        static gfx::GraphicsManager manager;
        static re::Scene scene;

        if(editor.edit())
        {
            editor.compile_assets(manager);
            scene = {};
            re::Entity e;
            e.vao = manager.vertex_array("triangle");
            e.program = manager.shader_program("triangle");
            e.texture = manager.texture("triangle");
            scene.add_entity(std::move(e));
        }
        scene.editor_ui(manager);

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

    glfwDestroyWindow(window);
    glfwTerminate();
}