

#pragma once

#include <AK/Assertions.h>
#include <AK/Checked.h>
#include <AK/Types.h>

namespace AK {

class Utf32View;

class Utf32CodepointIterator {
    friend class Utf32View;

public:
    Utf32CodepointIterator() = default;
    ~Utf32CodepointIterator() = default;

    bool operator==(const Utf32CodepointIterator& other) const
    {
        return m_ptr == other.m_ptr && m_length == other.m_length;
    }
    bool operator!=(const Utf32CodepointIterator& other) const
    {
        return !(*this == other);
    }
    Utf32CodepointIterator& operator++()
    {
        VERIFY(m_length > 0);
        m_ptr++;
        m_length--;
        return *this;
    }
    ssize_t operator-(const Utf32CodepointIterator& other) const
    {
        return m_ptr - other.m_ptr;
    }
    u32 operator*() const
    {
        VERIFY(m_length > 0);
        return *m_ptr;
    }

    constexpr int code_point_length_in_bytes() const { return sizeof(u32); }
    bool done() const { return !m_length; }

private:
    Utf32CodepointIterator(const u32* ptr, size_t length)
        : m_ptr(ptr)
        , m_length((ssize_t)length)
    {
    }
    const u32* m_ptr { nullptr };
    ssize_t m_length { -1 };
};

class Utf32View {
public:
    using Iterator = Utf32CodepointIterator;

    Utf32View() = default;
    Utf32View(const u32* code_points, size_t length)
        : m_code_points(code_points)
        , m_length(length)
    {
        VERIFY(code_points || length == 0);
    }

    Utf32CodepointIterator begin() const
    {
        return { begin_ptr(), m_length };
    }

    Utf32CodepointIterator end() const
    {
        return { end_ptr(), 0 };
    }

    const u32* code_points() const { return m_code_points; }
    bool is_empty() const { return m_length == 0; }
    size_t length() const { return m_length; }

    size_t iterator_offset(const Utf32CodepointIterator& it) const
    {
        VERIFY(it.m_ptr >= m_code_points);
        VERIFY(it.m_ptr < m_code_points + m_length);
        return ((ptrdiff_t)it.m_ptr - (ptrdiff_t)m_code_points) / sizeof(u32);
    }

    Utf32View substring_view(size_t offset, size_t length) const
    {
        if (length == 0)
            return {};
        VERIFY(offset < m_length);
        VERIFY(!Checked<size_t>::addition_would_overflow(offset, length));
        VERIFY((offset + length) <= m_length);
        return Utf32View(m_code_points + offset, length);
    }

private:
    const u32* begin_ptr() const
    {
        return m_code_points;
    }
    const u32* end_ptr() const
    {
        return m_code_points + m_length;
    }

    const u32* m_code_points { nullptr };
    size_t m_length { 0 };
};

}

using AK::Utf32View;
