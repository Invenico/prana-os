
#include <LibTest/TestCase.h>

#include <AK/IntrusiveList.h>
#include <AK/NonnullOwnPtr.h>
#include <AK/RefPtr.h>

class IntrusiveTestItem {
public:
    IntrusiveTestItem() = default;
    IntrusiveListNode<IntrusiveTestItem> m_list_node;
};
using IntrusiveTestList = IntrusiveList<IntrusiveTestItem, RawPtr<IntrusiveTestItem>, &IntrusiveTestItem::m_list_node>;

TEST_CASE(construct)
{
    IntrusiveTestList empty;
    EXPECT(empty.is_empty());
}

TEST_CASE(insert)
{
    IntrusiveTestList list;
    list.append(*new IntrusiveTestItem());

    EXPECT(!list.is_empty());

    delete list.take_last();
}

TEST_CASE(enumeration)
{
    constexpr size_t expected_size = 10;
    IntrusiveTestList list;
    for (size_t i = 0; i < expected_size; i++) {
        list.append(*new IntrusiveTestItem());
    }

    size_t actual_size = 0;
    for (auto& elem : list) {
        (void)elem;
        actual_size++;
    }
    EXPECT_EQ(expected_size, actual_size);
    while (auto elem = list.take_first()) {
        delete elem;
    }
}

class IntrusiveRefPtrItem : public RefCounted<IntrusiveRefPtrItem> {
public:
    IntrusiveRefPtrItem() = default;
    IntrusiveListNode<IntrusiveRefPtrItem, RefPtr<IntrusiveRefPtrItem>> m_list_node;
};
using IntrusiveRefPtrList = IntrusiveList<IntrusiveRefPtrItem, RefPtr<IntrusiveRefPtrItem>, &IntrusiveRefPtrItem::m_list_node>;

TEST_CASE(intrusive_ref_ptr_no_ref_leaks)
{
    auto item = adopt_ref(*new IntrusiveRefPtrItem());
    EXPECT_EQ(1u, item->ref_count());
    IntrusiveRefPtrList ref_list;

    ref_list.append(*item);
    EXPECT_EQ(2u, item->ref_count());

    ref_list.remove(*item);
    EXPECT_EQ(1u, item->ref_count());
}

TEST_CASE(intrusive_ref_ptr_clear)
{
    auto item = adopt_ref(*new IntrusiveRefPtrItem());
    EXPECT_EQ(1u, item->ref_count());
    IntrusiveRefPtrList ref_list;

    ref_list.append(*item);
    EXPECT_EQ(2u, item->ref_count());

    ref_list.clear();
    EXPECT_EQ(1u, item->ref_count());
}

TEST_CASE(intrusive_ref_ptr_destructor)
{
    auto item = adopt_ref(*new IntrusiveRefPtrItem());
    EXPECT_EQ(1u, item->ref_count());

    {
        IntrusiveRefPtrList ref_list;
        ref_list.append(*item);
        EXPECT_EQ(2u, item->ref_count());
    }

    EXPECT_EQ(1u, item->ref_count());
}

class IntrusiveNonnullRefPtrItem : public RefCounted<IntrusiveNonnullRefPtrItem> {
public:
    IntrusiveNonnullRefPtrItem() = default;
    IntrusiveListNode<IntrusiveNonnullRefPtrItem, NonnullRefPtr<IntrusiveNonnullRefPtrItem>> m_list_node;
};
using IntrusiveNonnullRefPtrList = IntrusiveList<IntrusiveNonnullRefPtrItem, NonnullRefPtr<IntrusiveNonnullRefPtrItem>, &IntrusiveNonnullRefPtrItem::m_list_node>;

TEST_CASE(intrusive_nonnull_ref_ptr_intrusive)
{
    auto item = adopt_ref(*new IntrusiveNonnullRefPtrItem());
    EXPECT_EQ(1u, item->ref_count());
    IntrusiveNonnullRefPtrList nonnull_ref_list;

    nonnull_ref_list.append(*item);
    EXPECT_EQ(2u, item->ref_count());
    EXPECT(!nonnull_ref_list.is_empty());

    nonnull_ref_list.remove(*item);
    EXPECT_EQ(1u, item->ref_count());

    EXPECT(nonnull_ref_list.is_empty());
}
