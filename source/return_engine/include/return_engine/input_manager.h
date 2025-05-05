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
        InputManager(GLFWwindow& window) : m_window(&window){}

        void update();
        bool get_key(Key) const;
        bool get_mouse_button(MouseButton) const;

        maths::Vector2 mouse_delta() const;
        maths::Vector2 mouse_pos() const;

    private:
        GLFWwindow* m_window;
        bool m_key_input_consumed_by_imgui = false;
        bool m_mouse_input_consumed_by_imgui = false;
        maths::Vector2 m_cursor_pos = { 0.f,0.f };
        maths::Vector2 m_cursor_delta = { 0.f,0.f };
    };
}