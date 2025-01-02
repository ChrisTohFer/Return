#include "input_manager.h"

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"

namespace re
{
    int g_key_conversion[(int)Key::Count] = 
    {
        GLFW_KEY_0,
        GLFW_KEY_1,
        GLFW_KEY_2,
        GLFW_KEY_3,
        GLFW_KEY_4,
        GLFW_KEY_5,
        GLFW_KEY_6,
        GLFW_KEY_7,
        GLFW_KEY_8,
        GLFW_KEY_9,

        GLFW_KEY_A,
        GLFW_KEY_B,
        GLFW_KEY_C,
        GLFW_KEY_D,
        GLFW_KEY_E,
        GLFW_KEY_F,
        GLFW_KEY_G,
        GLFW_KEY_H,
        GLFW_KEY_I,
        GLFW_KEY_J,
        GLFW_KEY_K,
        GLFW_KEY_L,
        GLFW_KEY_M,
        GLFW_KEY_N,
        GLFW_KEY_O,
        GLFW_KEY_P,
        GLFW_KEY_Q,
        GLFW_KEY_R,
        GLFW_KEY_S,
        GLFW_KEY_T,
        GLFW_KEY_U,
        GLFW_KEY_V,
        GLFW_KEY_W,
        GLFW_KEY_X,
        GLFW_KEY_Y,
        GLFW_KEY_Z,
    };

    int g_mouse_conversion[(int)MouseButton::Count] =
    {
        GLFW_MOUSE_BUTTON_1,
        GLFW_MOUSE_BUTTON_2,
        GLFW_MOUSE_BUTTON_3
    };

    void InputManager::update()
    {
        m_key_input_consumed_by_imgui = ImGui::GetIO().WantCaptureKeyboard;
        m_mouse_input_consumed_by_imgui = ImGui::GetIO().WantCaptureMouse;

        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        maths::Vector2 new_cursor_pos = { (float)x, (float)y };
        m_cursor_delta = new_cursor_pos - m_cursor_pos;
        m_cursor_pos = new_cursor_pos;
    }

    bool InputManager::get_key(Key key) const
    {
        return !m_key_input_consumed_by_imgui && glfwGetKey(m_window, g_key_conversion[(int)key]);
    }

    bool InputManager::get_mouse_button(MouseButton button) const
    {
        return !m_mouse_input_consumed_by_imgui && glfwGetMouseButton(m_window, g_mouse_conversion[(int)button]);
    }

    maths::Vector2 InputManager::mouse_delta() const
    {
        return m_cursor_delta;
    }

    maths::Vector2 InputManager::mouse_pos() const
    {
        return m_cursor_pos;
    }
}