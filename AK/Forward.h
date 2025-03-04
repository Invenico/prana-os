
#pragma once

#include <AK/Types.h>

namespace AK {

class Bitmap;
class ByteBuffer;
class IPv4Address;
class JsonArray;
class JsonObject;
class JsonValue;
class StackInfo;
class String;
class StringBuilder;
class StringImpl;
class StringView;
class Time;
class URL;
class FlyString;
class Utf32View;
class Utf8View;
class InputStream;
class InputMemoryStream;
class DuplexMemoryStream;
class OutputStream;
class InputBitStream;
class OutputBitStream;
class OutputMemoryStream;

template<size_t Capacity>
class CircularDuplexStream;

template<typename T>
class Span;

template<typename T, size_t Size>
struct Array;

template<typename Container, typename ValueType>
class SimpleIterator;

using ReadonlyBytes = Span<const u8>;
using Bytes = Span<u8>;

template<typename T, AK::MemoryOrder DefaultMemoryOrder>
class Atomic;

template<typename T>
class SinglyLinkedList;

template<typename T>
class DoublyLinkedList;

template<typename T>
class InlineLinkedList;

template<typename T, size_t capacity>
class CircularQueue;

template<typename T>
struct Traits;

template<typename T, typename = Traits<T>>
class HashTable;

template<typename K, typename V, typename = Traits<K>>
class HashMap;

template<typename T>
class Badge;

template<typename>
class Function;

template<typename Out, typename... In>
class Function<Out(In...)>;

template<typename T>
class NonnullRefPtr;

template<typename T>
class NonnullOwnPtr;

template<typename T, size_t inline_capacity = 0>
class NonnullRefPtrVector;

template<typename T, size_t inline_capacity = 0>
class NonnullOwnPtrVector;

template<typename T>
class Optional;

template<typename T>
struct RefPtrTraits;

template<typename T, typename PtrTraits = RefPtrTraits<T>>
class RefPtr;

template<typename T>
class OwnPtr;

template<typename T>
class WeakPtr;

template<typename T, size_t inline_capacity = 0>
class Vector;

}

using AK::Array;
using AK::Atomic;
using AK::Badge;
using AK::Bitmap;
using AK::ByteBuffer;
using AK::Bytes;
using AK::CircularDuplexStream;
using AK::CircularQueue;
using AK::DoublyLinkedList;
using AK::DuplexMemoryStream;
using AK::FlyString;
using AK::Function;
using AK::HashMap;
using AK::HashTable;
using AK::InlineLinkedList;
using AK::InputBitStream;
using AK::InputMemoryStream;
using AK::InputStream;
using AK::IPv4Address;
using AK::JsonArray;
using AK::JsonObject;
using AK::JsonValue;
using AK::NonnullOwnPtr;
using AK::NonnullOwnPtrVector;
using AK::NonnullRefPtr;
using AK::NonnullRefPtrVector;
using AK::Optional;
using AK::OutputBitStream;
using AK::OutputMemoryStream;
using AK::OutputStream;
using AK::OwnPtr;
using AK::ReadonlyBytes;
using AK::RefPtr;
using AK::SinglyLinkedList;
using AK::Span;
using AK::StackInfo;
using AK::String;
using AK::StringBuilder;
using AK::StringImpl;
using AK::StringView;
using AK::Time;
using AK::Traits;
using AK::URL;
using AK::Utf32View;
using AK::Utf8View;
using AK::Vector;
