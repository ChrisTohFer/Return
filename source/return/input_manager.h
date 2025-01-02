#pragma once

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

    class InputManager
    {
    public:
        InputManager(GLFWwindow& window) : m_window(&window){}

        void update();
        bool get_key(Key) const;
    private:
        GLFWwindow* m_window;
        bool m_input_consumed_by_imgui = false;
    };
}