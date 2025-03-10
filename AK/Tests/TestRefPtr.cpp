
#include <LibTest/TestCase.h>

#include <AK/NonnullRefPtr.h>
#include <AK/String.h>

struct Object : public RefCounted<Object> {
    int x;
};

struct Object2 : Object {
};

struct SelfAwareObject : public RefCounted<SelfAwareObject> {
    void one_ref_left() { m_has_one_ref_left = true; }
    void will_be_destroyed() { ++num_destroyed; }

    bool m_has_one_ref_left = false;
    static size_t num_destroyed;
};
size_t SelfAwareObject::num_destroyed = 0;

TEST_CASE(basics)
{
    RefPtr<Object> object = adopt_ref(*new Object);
    EXPECT(object.ptr() != nullptr);
    EXPECT_EQ(object->ref_count(), 1u);
    object->ref();
    EXPECT_EQ(object->ref_count(), 2u);
    object->unref();
    EXPECT_EQ(object->ref_count(), 1u);

    {
        NonnullRefPtr another = *object;
        EXPECT_EQ(object->ref_count(), 2u);
    }

    EXPECT_EQ(object->ref_count(), 1u);
}

TEST_CASE(assign_reference)
{
    RefPtr<Object> object = adopt_ref(*new Object);
    EXPECT_EQ(object->ref_count(), 1u);
    object = *object;
    EXPECT_EQ(object->ref_count(), 1u);
}

TEST_CASE(assign_ptr)
{
    RefPtr<Object> object = adopt_ref(*new Object);
    EXPECT_EQ(object->ref_count(), 1u);
    object = object.ptr();
    EXPECT_EQ(object->ref_count(), 1u);
}

TEST_CASE(copy_move_ref)
{
    RefPtr<Object2> object = adopt_ref(*new Object2);
    EXPECT_EQ(object->ref_count(), 1u);
    {
        auto object2 = object;
        EXPECT_EQ(object->ref_count(), 2u);

        RefPtr<Object> object1 = object;
        EXPECT_EQ(object->ref_count(), 3u);

        object1 = move(object2);
        EXPECT_EQ(object->ref_count(), 2u);

        RefPtr<Object> object3(move(object1));
        EXPECT_EQ(object3->ref_count(), 2u);

        object1 = object3;
        EXPECT_EQ(object3->ref_count(), 3u);
    }
    EXPECT_EQ(object->ref_count(), 1u);
}

TEST_CASE(swap)
{
    RefPtr<Object> object_a = adopt_ref(*new Object);
    RefPtr<Object> object_b = adopt_ref(*new Object);
    auto* ptr_a = object_a.ptr();
    auto* ptr_b = object_b.ptr();
    swap(object_a, object_b);
    EXPECT_EQ(object_a, ptr_b);
    EXPECT_EQ(object_b, ptr_a);
    EXPECT_EQ(object_a->ref_count(), 1u);
    EXPECT_EQ(object_b->ref_count(), 1u);
}

TEST_CASE(assign_moved_self)
{
    RefPtr<Object> object = adopt_ref(*new Object);
    EXPECT_EQ(object->ref_count(), 1u);
#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wself-move"
#endif
    object = move(object);
#ifdef __clang__
#    pragma clang diagnostic pop
#endif
    EXPECT_EQ(object->ref_count(), 1u);
}

TEST_CASE(assign_copy_self)
{
    RefPtr<Object> object = adopt_ref(*new Object);
    EXPECT_EQ(object->ref_count(), 1u);

#ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
    object = object;
#ifdef __clang__
#    pragma clang diagnostic pop
#endif

    EXPECT_EQ(object->ref_count(), 1u);
}

TEST_CASE(self_observers)
{
    RefPtr<SelfAwareObject> object = adopt_ref(*new SelfAwareObject);
    EXPECT_EQ(object->ref_count(), 1u);
    EXPECT_EQ(object->m_has_one_ref_left, false);
    EXPECT_EQ(SelfAwareObject::num_destroyed, 0u);

    object->ref();
    EXPECT_EQ(object->ref_count(), 2u);
    EXPECT_EQ(object->m_has_one_ref_left, false);
    EXPECT_EQ(SelfAwareObject::num_destroyed, 0u);

    object->unref();
    EXPECT_EQ(object->ref_count(), 1u);
    EXPECT_EQ(object->m_has_one_ref_left, true);
    EXPECT_EQ(SelfAwareObject::num_destroyed, 0u);

    object->unref();
    EXPECT_EQ(SelfAwareObject::num_destroyed, 1u);
}
