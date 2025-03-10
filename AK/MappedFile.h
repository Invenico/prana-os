
#pragma once

#include <AK/Noncopyable.h>
#include <AK/NonnullRefPtr.h>
#include <AK/OSError.h>
#include <AK/RefCounted.h>
#include <AK/Result.h>

namespace AK {

class MappedFile : public RefCounted<MappedFile> {
    AK_MAKE_NONCOPYABLE(MappedFile);
    AK_MAKE_NONMOVABLE(MappedFile);

public:
    static Result<NonnullRefPtr<MappedFile>, OSError> map(const String& path);
    ~MappedFile();

    void* data() { return m_data; }
    const void* data() const { return m_data; }

    size_t size() const { return m_size; }

    ReadonlyBytes bytes() const { return { m_data, m_size }; }

private:
    explicit MappedFile(void*, size_t);

    void* m_data { nullptr };
    size_t m_size { 0 };
};

}

using AK::MappedFile;
