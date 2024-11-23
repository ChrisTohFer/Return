#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

namespace r
{
    //expanding fifo container. cannot shrink.
    template<typename ElementType>
    class ExpandingQueue
    {
    public:
        ExpandingQueue(int initial_size = 8) : m_elements(initial_size) {}

        void push(const ElementType&);
        void push(ElementType&&);
        ElementType pop();

        int count() const { return m_count; }
    private:
        void expand_if_full();
        int back_of_line() { return (m_next_in_line + m_count) % m_elements.size(); }

        std::vector<ElementType> m_elements;
        int m_next_in_line = 0;
        int m_count = 0;
    };

    //inline definitions

    template<typename ElementType>
    inline void ExpandingQueue<ElementType>::push(const ElementType& element)
    {
        expand_if_full();
        m_elements[back_of_line()] = element;
        ++m_count;
    }

    template<typename ElementType>
    inline void ExpandingQueue<ElementType>::push(ElementType&& element)
    {
        expand_if_full();
        m_elements[back_of_line()] = std::move(element);
        ++m_count;
    }

    template<typename ElementType>
    inline ElementType ExpandingQueue<ElementType>::pop()
    {
        assert(m_count > 0);
        auto element = std::move(m_elements[m_next_in_line]);
        m_next_in_line = (m_next_in_line + 1) % m_elements.size();
        --m_count;
        return element;
    }

    template<typename ElementType>
    inline void ExpandingQueue<ElementType>::expand_if_full()
    {
        if (m_count == m_elements.size())
        {
            if (m_next_in_line != 0)
            {
                //rotate elements to begin at the start of the container so they remain contiguous
                std::rotate(m_elements.begin(), m_elements.begin() + m_next_in_line, m_elements.end());
                m_next_in_line = 0;
            }

            constexpr int expand_factor = 2;
            m_elements.resize(m_elements.size() * expand_factor);
        }
    }

}