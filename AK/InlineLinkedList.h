
#pragma once

#include <AK/Assertions.h>
#include <AK/Types.h>

namespace AK {

template<typename T>
class InlineLinkedList;

template<typename T>
class InlineLinkedListIterator {
public:
    bool operator!=(const InlineLinkedListIterator& other) const { return m_node != other.m_node; }
    bool operator==(const InlineLinkedListIterator& other) const { return m_node == other.m_node; }
    InlineLinkedListIterator& operator++()
    {
        m_node = m_node->next();
        return *this;
    }
    T& operator*() { return *m_node; }
    T* operator->() { return m_node; }
    bool is_end() const { return !m_node; }
    static InlineLinkedListIterator universal_end() { return InlineLinkedListIterator(nullptr); }

private:
    friend InlineLinkedList<T>;
    explicit InlineLinkedListIterator(T* node)
        : m_node(node)
    {
    }
    T* m_node;
};

template<typename T>
class InlineLinkedListNode {
public:
    InlineLinkedListNode();

    void set_prev(T*);
    void set_next(T*);

    T* prev() const;
    T* next() const;
};

template<typename T>
inline InlineLinkedListNode<T>::InlineLinkedListNode()
{
    set_prev(0);
    set_next(0);
}

template<typename T>
inline void InlineLinkedListNode<T>::set_prev(T* prev)
{
    static_cast<T*>(this)->m_prev = prev;
}

template<typename T>
inline void InlineLinkedListNode<T>::set_next(T* next)
{
    static_cast<T*>(this)->m_next = next;
}

template<typename T>
inline T* InlineLinkedListNode<T>::prev() const
{
    return static_cast<const T*>(this)->m_prev;
}

template<typename T>
inline T* InlineLinkedListNode<T>::next() const
{
    return static_cast<const T*>(this)->m_next;
}

template<typename T>
class InlineLinkedList {
public:
    InlineLinkedList() = default;

    bool is_empty() const { return !m_head; }
    size_t size_slow() const;
    void clear();

    T* head() const { return m_head; }
    T* remove_head();
    T* remove_tail();

    T* tail() const { return m_tail; }

    void prepend(T*);
    void append(T*);
    void remove(T*);
    void append(InlineLinkedList<T>&);
    void insert_before(T*, T*);
    void insert_after(T*, T*);

    bool contains_slow(T* value) const
    {
        for (T* node = m_head; node; node = node->next()) {
            if (node == value)
                return true;
        }
        return false;
    }

    template<typename F>
    IterationDecision for_each(F func) const
    {
        for (T* node = m_head; node; node = node->next()) {
            IterationDecision decision = func(*node);
            if (decision != IterationDecision::Continue)
                return decision;
        }
        return IterationDecision::Continue;
    }

    using Iterator = InlineLinkedListIterator<T>;
    friend Iterator;
    Iterator begin() { return Iterator(m_head); }
    Iterator end() { return Iterator::universal_end(); }

    using ConstIterator = InlineLinkedListIterator<const T>;
    friend ConstIterator;
    ConstIterator begin() const { return ConstIterator(m_head); }
    ConstIterator end() const { return ConstIterator::universal_end(); }

private:
    T* m_head { nullptr };
    T* m_tail { nullptr };
};

template<typename T>
inline size_t InlineLinkedList<T>::size_slow() const
{
    size_t size = 0;
    for (T* node = m_head; node; node = node->next())
        ++size;
    return size;
}

template<typename T>
inline void InlineLinkedList<T>::clear()
{
    m_head = 0;
    m_tail = 0;
}

template<typename T>
inline void InlineLinkedList<T>::prepend(T* node)
{
    if (!m_head) {
        VERIFY(!m_tail);
        m_head = node;
        m_tail = node;
        node->set_prev(0);
        node->set_next(0);
        return;
    }

    VERIFY(m_tail);
    m_head->set_prev(node);
    node->set_next(m_head);
    node->set_prev(0);
    m_head = node;
}

template<typename T>
inline void InlineLinkedList<T>::append(T* node)
{
    if (!m_tail) {
        VERIFY(!m_head);
        m_head = node;
        m_tail = node;
        node->set_prev(0);
        node->set_next(0);
        return;
    }

    VERIFY(m_head);
    m_tail->set_next(node);
    node->set_prev(m_tail);
    node->set_next(0);
    m_tail = node;
}

template<typename T>
inline void InlineLinkedList<T>::insert_before(T* before_node, T* node)
{
    VERIFY(before_node);
    VERIFY(node);
    VERIFY(before_node != node);
    VERIFY(!is_empty());
    if (m_head == before_node) {
        VERIFY(!before_node->prev());
        m_head = node;
        node->set_prev(0);
        node->set_next(before_node);
        before_node->set_prev(node);
    } else {
        VERIFY(before_node->prev());
        node->set_prev(before_node->prev());
        before_node->prev()->set_next(node);
        node->set_next(before_node);
        before_node->set_prev(node);
    }
}

template<typename T>
inline void InlineLinkedList<T>::insert_after(T* after_node, T* node)
{
    VERIFY(after_node);
    VERIFY(node);
    VERIFY(after_node != node);
    VERIFY(!is_empty());
    if (m_tail == after_node) {
        VERIFY(!after_node->next());
        m_tail = node;
        node->set_prev(after_node);
        node->set_next(0);
        after_node->set_next(node);
    } else {
        VERIFY(after_node->next());
        node->set_prev(after_node);
        node->set_next(after_node->next());
        after_node->next()->set_prev(node);
        after_node->set_next(node);
    }
}

template<typename T>
inline void InlineLinkedList<T>::remove(T* node)
{
    if (node->prev()) {
        VERIFY(node != m_head);
        node->prev()->set_next(node->next());
    } else {
        VERIFY(node == m_head);
        m_head = node->next();
    }

    if (node->next()) {
        VERIFY(node != m_tail);
        node->next()->set_prev(node->prev());
    } else {
        VERIFY(node == m_tail);
        m_tail = node->prev();
    }

    node->set_next(0);
    node->set_prev(0);
}

template<typename T>
inline T* InlineLinkedList<T>::remove_head()
{
    T* node = head();
    if (node)
        remove(node);
    return node;
}

template<typename T>
inline T* InlineLinkedList<T>::remove_tail()
{
    T* node = tail();
    if (node)
        remove(node);
    return node;
}

template<typename T>
inline void InlineLinkedList<T>::append(InlineLinkedList<T>& other)
{
    if (!other.head())
        return;

    if (!head()) {
        m_head = other.head();
        m_tail = other.tail();
        other.clear();
        return;
    }

    VERIFY(tail());
    VERIFY(other.head());
    T* other_head = other.head();
    T* other_tail = other.tail();
    other.clear();

    VERIFY(!m_tail->next());
    m_tail->set_next(other_head);
    VERIFY(!other_head->prev());
    other_head->set_prev(m_tail);
    m_tail = other_tail;
}

}

using AK::InlineLinkedList;
using AK::InlineLinkedListNode;
