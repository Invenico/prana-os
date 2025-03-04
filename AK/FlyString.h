
#pragma once

#include <AK/String.h>

namespace AK {

class FlyString {
public:
    FlyString() = default;
    FlyString(const FlyString& other)
        : m_impl(other.impl())
    {
    }
    FlyString(FlyString&& other)
        : m_impl(move(other.m_impl))
    {
    }
    FlyString(const String&);
    FlyString(const StringView&);
    FlyString(const char*);

    FlyString& operator=(const FlyString& other)
    {
        m_impl = other.m_impl;
        return *this;
    }

    FlyString& operator=(FlyString&& other)
    {
        m_impl = move(other.m_impl);
        return *this;
    }

    bool is_empty() const { return !m_impl || !m_impl->length(); }
    bool is_null() const { return !m_impl; }

    bool operator==(const FlyString& other) const { return m_impl == other.m_impl; }
    bool operator!=(const FlyString& other) const { return m_impl != other.m_impl; }

    bool operator==(const String&) const;
    bool operator!=(const String& string) const { return !(*this == string); }

    bool operator==(const StringView&) const;
    bool operator!=(const StringView& string) const { return !(*this == string); }

    bool operator==(const char*) const;
    bool operator!=(const char* string) const { return !(*this == string); }

    const StringImpl* impl() const { return m_impl; }
    const char* characters() const { return m_impl ? m_impl->characters() : nullptr; }
    size_t length() const { return m_impl ? m_impl->length() : 0; }

    ALWAYS_INLINE u32 hash() const { return m_impl ? m_impl->existing_hash() : 0; }

    StringView view() const;

    FlyString to_lowercase() const;

    template<typename T = int>
    Optional<T> to_int() const;
    template<typename T = unsigned>
    Optional<T> to_uint() const;

    bool equals_ignoring_case(const StringView&) const;
    bool starts_with(const StringView&, CaseSensitivity = CaseSensitivity::CaseSensitive) const;
    bool ends_with(const StringView&, CaseSensitivity = CaseSensitivity::CaseSensitive) const;

    static void did_destroy_impl(Badge<StringImpl>, StringImpl&);

    template<typename T, typename... Rest>
    bool is_one_of(const T& string, Rest... rest) const
    {
        if (*this == string)
            return true;
        return is_one_of(rest...);
    }

private:
    bool is_one_of() const { return false; }

    RefPtr<StringImpl> m_impl;
};

template<>
struct Traits<FlyString> : public GenericTraits<FlyString> {
    static unsigned hash(const FlyString& s) { return s.hash(); }
};

}

using AK::FlyString;
