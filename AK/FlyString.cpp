#include <AK/FlyString.h>
#include <AK/HashTable.h>
#include <AK/Optional.h>
#include <AK/Singleton.h>
#include <AK/String.h>
#include <AK/StringUtils.h>
#include <AK/StringView.h>

namespace AK {

struct FlyStringImplTraits : public Traits<StringImpl*> {
    static unsigned hash(const StringImpl* s) { return s ? s->hash() : 0; }
    static bool equals(const StringImpl* a, const StringImpl* b)
    {
        VERIFY(a);
        VERIFY(b);
        return *a == *b;
    }
};

static AK::Singleton<HashTable<StringImpl*, FlyStringImplTraits>> s_table;

static HashTable<StringImpl*, FlyStringImplTraits>& fly_impls()
{
    return *s_table;
}

void FlyString::did_destroy_impl(Badge<StringImpl>, StringImpl& impl)
{
    fly_impls().remove(&impl);
}

FlyString::FlyString(const String& string)
{
    if (string.is_null())
        return;
    if (string.impl()->is_fly()) {
        m_impl = string.impl();
        return;
    }
    auto it = fly_impls().find(const_cast<StringImpl*>(string.impl()));
    if (it == fly_impls().end()) {
        fly_impls().set(const_cast<StringImpl*>(string.impl()));
        string.impl()->set_fly({}, true);
        m_impl = string.impl();
    } else {
        VERIFY((*it)->is_fly());
        m_impl = *it;
    }
}

FlyString::FlyString(const StringView& string)
    : FlyString(static_cast<String>(string))
{
}

FlyString::FlyString(const char* string)
    : FlyString(static_cast<String>(string))
{
}

template<typename T>
Optional<T> FlyString::to_int() const
{
    return StringUtils::convert_to_int<T>(view());
}

template Optional<i8> FlyString::to_int() const;
template Optional<i16> FlyString::to_int() const;
template Optional<i32> FlyString::to_int() const;
template Optional<i64> FlyString::to_int() const;

template<typename T>
Optional<T> FlyString::to_uint() const
{
    return StringUtils::convert_to_uint<T>(view());
}

template Optional<u8> FlyString::to_uint() const;
template Optional<u16> FlyString::to_uint() const;
template Optional<u32> FlyString::to_uint() const;
template Optional<u64> FlyString::to_uint() const;

bool FlyString::equals_ignoring_case(const StringView& other) const
{
    return StringUtils::equals_ignoring_case(view(), other);
}

bool FlyString::starts_with(const StringView& str, CaseSensitivity case_sensitivity) const
{
    return StringUtils::starts_with(view(), str, case_sensitivity);
}

bool FlyString::ends_with(const StringView& str, CaseSensitivity case_sensitivity) const
{
    return StringUtils::ends_with(view(), str, case_sensitivity);
}

FlyString FlyString::to_lowercase() const
{
    return String(*m_impl).to_lowercase();
}

StringView FlyString::view() const
{
    return { characters(), length() };
}

bool FlyString::operator==(const String& other) const
{
    if (m_impl == other.impl())
        return true;

    if (!m_impl)
        return !other.impl();

    if (!other.impl())
        return false;

    if (length() != other.length())
        return false;

    return !__builtin_memcmp(characters(), other.characters(), length());
}

bool FlyString::operator==(const StringView& string) const
{
    return *this == String(string);
}

bool FlyString::operator==(const char* string) const
{
    if (is_null())
        return !string;
    if (!string)
        return false;
    return !__builtin_strcmp(m_impl->characters(), string);
}

}
