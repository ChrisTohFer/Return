#include "task_manager.h"

#include "graphics_test.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <iostream>


void error_callback(int error, const char* description)
{
    printf("Error: %s\n", description);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    constexpr int winx = 1000, winy = 700;
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

    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.f, 0.f, 0.f, 255.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::ShowDemoWindow();
        static GraphicsTestEditor editor;
        editor.edit();

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