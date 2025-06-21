#pragma once

#include "maths/vector2.h"

struct GLFWwindow;

namespace re
{
    enum class Key
    {
        Top0,
        Top1,
        Top2,
        Top3,
        Top4,
        Top5,
        Top6,
        Top7,
        Top8,
        Top9,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        Space,
        LControl,

        Count
    };

    enum class MouseButton
    {
        Left,
        Right,
        Middle,

        Count
    };

    class InputManager
    {
    public:
        InputManager(GLFWwindow& window);

        void update();
        bool key_pressed(Key) const;
        bool key_up(Key) const;
        bool key_down(Key) const;
        bool get_mouse_button(MouseButton) const;

        maths::Vector2 mouse_delta() const;
        maths::Vector2 mouse_pos() const;

        void set_down(Key);
        void set_up(Key);

    private:
        GLFWwindow* m_window;
        bool m_key_input_consumed_by_imgui = false;
        bool m_mouse_input_consumed_by_imgui = false;
        maths::Vector2 m_cursor_pos = { 0.f,0.f };
        maths::Vector2 m_cursor_delta = { 0.f,0.f };


        static_assert((int)Key::Count < 64);
        uint64_t m_key_pressed_buffers[2] = { 0,0 };
        uint64_t m_key_down_buffers[2] = { 0,0 };
        uint64_t m_key_up_buffers[2] = { 0,0 };

        //alternates each frame, key events fill the inactive buffers
        bool m_active_buffer : 1 = true;
        bool m_inactive_buffer : 1 = false;
    };
}