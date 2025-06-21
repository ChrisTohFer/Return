#include "input_manager.h"

#include "GLFW/glfw3.h"
#include "imgui/imgui.h"

#include <array>

namespace re
{
    static constexpr std::array<Key, GLFW_KEY_LAST> g_glfw_to_key_conversion = []()
        {
            std::array<Key, GLFW_KEY_LAST> conversions;
            conversions.fill(Key::Count);

            conversions[GLFW_KEY_0]            = Key::Top0;
            conversions[GLFW_KEY_1]            = Key::Top1;
            conversions[GLFW_KEY_2]            = Key::Top2;
            conversions[GLFW_KEY_3]            = Key::Top3;
            conversions[GLFW_KEY_4]            = Key::Top4;
            conversions[GLFW_KEY_5]            = Key::Top5;
            conversions[GLFW_KEY_6]            = Key::Top6;
            conversions[GLFW_KEY_7]            = Key::Top7;
            conversions[GLFW_KEY_8]            = Key::Top8;
            conversions[GLFW_KEY_9]            = Key::Top9;
            conversions[GLFW_KEY_A]            = Key::A;
            conversions[GLFW_KEY_B]            = Key::B;
            conversions[GLFW_KEY_C]            = Key::C;
            conversions[GLFW_KEY_D]            = Key::D;
            conversions[GLFW_KEY_E]            = Key::E;
            conversions[GLFW_KEY_F]            = Key::F;
            conversions[GLFW_KEY_G]            = Key::G;
            conversions[GLFW_KEY_H]            = Key::H;
            conversions[GLFW_KEY_I]            = Key::I;
            conversions[GLFW_KEY_J]            = Key::J;
            conversions[GLFW_KEY_K]            = Key::K;
            conversions[GLFW_KEY_L]            = Key::L;
            conversions[GLFW_KEY_M]            = Key::M;
            conversions[GLFW_KEY_N]            = Key::N;
            conversions[GLFW_KEY_O]            = Key::O;
            conversions[GLFW_KEY_P]            = Key::P;
            conversions[GLFW_KEY_Q]            = Key::Q;
            conversions[GLFW_KEY_R]            = Key::R;
            conversions[GLFW_KEY_S]            = Key::S;
            conversions[GLFW_KEY_T]            = Key::T;
            conversions[GLFW_KEY_U]            = Key::U;
            conversions[GLFW_KEY_V]            = Key::V;
            conversions[GLFW_KEY_W]            = Key::W;
            conversions[GLFW_KEY_X]            = Key::X;
            conversions[GLFW_KEY_Y]            = Key::Y;
            conversions[GLFW_KEY_Z]            = Key::Z;
            conversions[GLFW_KEY_SPACE]        = Key::Space;
            conversions[GLFW_KEY_LEFT_CONTROL] = Key::LControl;

            return conversions;
        }();

    static constexpr int g_key_to_glfw_conversion[(int)Key::Count] = 
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

        GLFW_KEY_SPACE,
        GLFW_KEY_LEFT_CONTROL,
    };

    static constexpr int g_mouse_to_glfw_conversion[(int)MouseButton::Count] =
    {
        GLFW_MOUSE_BUTTON_1,
        GLFW_MOUSE_BUTTON_2,
        GLFW_MOUSE_BUTTON_3
    };

    static uint64_t key_flag(Key key)
    {
        return 1ull << (int)key;
    }

    //input manager should be a namespace if this is how it's gonna be...
    static InputManager* g_input_manager = nullptr;

    static void key_callback(GLFWwindow*, int key, int, int action, int)
    {
        if (action == GLFW_PRESS) g_input_manager->set_down(g_glfw_to_key_conversion[key]);
        if (action == GLFW_RELEASE) g_input_manager->set_up(g_glfw_to_key_conversion[key]);
    }

    InputManager::InputManager(GLFWwindow& window)
        : m_window(&window)
    {
        assert(g_input_manager == nullptr);
        g_input_manager = this;
        glfwSetKeyCallback(&window, key_callback);
    }

    void InputManager::update()
    {
        m_key_input_consumed_by_imgui = ImGui::GetIO().WantCaptureKeyboard;
        m_mouse_input_consumed_by_imgui = ImGui::GetIO().WantCaptureMouse;

        double x, y;
        glfwGetCursorPos(m_window, &x, &y);
        maths::Vector2 new_cursor_pos = { (float)x, (float)y };
        m_cursor_delta = new_cursor_pos - m_cursor_pos;
        m_cursor_pos = new_cursor_pos;

        //this section probably needs to be thread safe
        
        //swap the active and inactive buffers
        m_active_buffer = !m_active_buffer;
        m_inactive_buffer = !m_inactive_buffer;

        //clear key down and key up buffers
        m_key_down_buffers[m_inactive_buffer] = 0;
        m_key_up_buffers[m_inactive_buffer] = 0;

        //key pressed buffer is carried over from the previous buffer
        m_key_pressed_buffers[m_inactive_buffer] = m_key_pressed_buffers[m_active_buffer];
    }

    bool InputManager::key_pressed(Key key) const
    {
        return !m_key_input_consumed_by_imgui && (m_key_pressed_buffers[m_active_buffer] & key_flag(key));
    }

    bool InputManager::key_down(Key key) const
    {
        return !m_key_input_consumed_by_imgui && (m_key_down_buffers[m_active_buffer] & key_flag(key));
    }

    bool InputManager::key_up(Key key) const
    {
        return !m_key_input_consumed_by_imgui && (m_key_up_buffers[m_active_buffer] & key_flag(key));
    }

    bool InputManager::get_mouse_button(MouseButton button) const
    {
        return !m_mouse_input_consumed_by_imgui && glfwGetMouseButton(m_window, g_mouse_to_glfw_conversion[(int)button]);
    }

    maths::Vector2 InputManager::mouse_delta() const
    {
        return m_cursor_delta;
    }

    maths::Vector2 InputManager::mouse_pos() const
    {
        return m_cursor_pos;
    }

    void InputManager::set_down(Key key)
    {
        if (key == Key::Count) return;

        m_key_down_buffers[m_inactive_buffer] |= key_flag(key);
        m_key_pressed_buffers[m_inactive_buffer] |= key_flag(key);
    }

    void InputManager::set_up(Key key)
    {
        if (key == Key::Count) return;

        m_key_up_buffers[m_inactive_buffer] |= key_flag(key);
        m_key_pressed_buffers[m_inactive_buffer] &= ~key_flag(key);
    }
}