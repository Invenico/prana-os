
#pragma once

#include <AK/Assertions.h>
#include <AK/Optional.h>

namespace AK {

template<typename ValueType, typename ErrorType>
class [[nodiscard]] Result {
public:
    Result(const ValueType& res)
        : m_result(res)
    {
    }

    Result(ValueType&& res)
        : m_result(move(res))
    {
    }

    Result(const ErrorType& error)
        : m_error(error)
    {
    }

    Result(ErrorType&& error)
        : m_error(move(error))
    {
    }

    Result(Result&& other) = default;
    Result(const Result& other) = default;
    ~Result() = default;

    ValueType& value()
    {
        return m_result.value();
    }

    ErrorType& error()
    {
        return m_error.value();
    }

    bool is_error() const
    {
        return m_error.has_value();
    }

    ValueType release_value()
    {
        return m_result.release_value();
    }

    ErrorType release_error()
    {
        return m_error.release_value();
    }

private:
    Optional<ValueType> m_result;
    Optional<ErrorType> m_error;
};

// Partial specialization for void value type
template<typename ErrorType>
class [[nodiscard]] Result<void, ErrorType> {
public:
    Result(const ErrorType& error)
        : m_error(error)
    {
    }

    Result(ErrorType&& error)
        : m_error(move(error))
    {
    }

    Result() = default;
    Result(Result&& other) = default;
    Result(const Result& other) = default;
    ~Result() = default;

    ErrorType& error()
    {
        return m_error.value();
    }

    bool is_error() const
    {
        return m_error.has_value();
    }

    ErrorType release_error()
    {
        return m_error.release_value();
    }

private:
    Optional<ErrorType> m_error;
};

}

using AK::Result;
