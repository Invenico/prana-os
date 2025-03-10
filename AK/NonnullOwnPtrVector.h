
#pragma once

#include <AK/NonnullOwnPtr.h>
#include <AK/NonnullPtrVector.h>

namespace AK {

template<typename T, size_t inline_capacity>
class NonnullOwnPtrVector : public NonnullPtrVector<NonnullOwnPtr<T>, inline_capacity> {
};

}

using AK::NonnullOwnPtrVector;
