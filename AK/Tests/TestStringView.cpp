
#include <LibTest/TestCase.h>

#include <AK/String.h>

TEST_CASE(construct_empty)
{
    EXPECT(StringView().is_null());
    EXPECT(StringView().is_empty());
    EXPECT(!StringView().characters_without_null_termination());
    EXPECT_EQ(StringView().length(), 0u);
}

TEST_CASE(view_literal)
{
    const char* truth = "cats rule dogs drool";
    StringView view(truth);
    EXPECT_EQ(view.is_null(), false);
    EXPECT_EQ(view.characters_without_null_termination(), truth);
    EXPECT_EQ(view, view);
    EXPECT_EQ(view, truth);
}

TEST_CASE(compare_views)
{
    String foo1 = "foo";
    String foo2 = "foo";
    auto view1 = foo1.view();
    auto view2 = foo2.view();

    EXPECT_EQ(view1, view2);
    EXPECT_EQ(view1, foo1);
    EXPECT_EQ(view1, foo2);
    EXPECT_EQ(view1, "foo");
}

TEST_CASE(string_view_literal_operator)
{
    StringView literal_view = "foo"sv;
    String test_string = "foo";

    EXPECT_EQ(literal_view.length(), test_string.length());
    EXPECT_EQ(literal_view, test_string);
}

TEST_CASE(starts_with)
{
    String test_string = "ABCDEF";
    StringView test_string_view = test_string.view();
    EXPECT(test_string_view.starts_with('A'));
    EXPECT(!test_string_view.starts_with('B'));
    EXPECT(test_string_view.starts_with("AB"));
    EXPECT(test_string_view.starts_with("ABCDEF"));
    EXPECT(!test_string_view.starts_with("DEF"));
    EXPECT(test_string_view.starts_with("abc", CaseSensitivity::CaseInsensitive));
    EXPECT(!test_string_view.starts_with("abc", CaseSensitivity::CaseSensitive));
}

TEST_CASE(ends_with)
{
    String test_string = "ABCDEF";
    StringView test_string_view = test_string.view();
    EXPECT(test_string_view.ends_with("DEF"));
    EXPECT(test_string_view.ends_with('F'));
    EXPECT(!test_string_view.ends_with('E'));
    EXPECT(test_string_view.ends_with("ABCDEF"));
    EXPECT(!test_string_view.ends_with("ABCDE"));
    EXPECT(!test_string_view.ends_with("ABCDEFG"));
    EXPECT(test_string_view.ends_with("def", CaseSensitivity::CaseInsensitive));
    EXPECT(!test_string_view.ends_with("def", CaseSensitivity::CaseSensitive));
}

TEST_CASE(lines)
{
    String test_string = "a\nb\r\nc\rd";
    StringView test_string_view = test_string.view();
    Vector<StringView> test_string_vector = test_string_view.lines();
    EXPECT_EQ(test_string_vector.size(), 4u);
    EXPECT(test_string_vector.at(0) == String("a"));
    EXPECT(test_string_vector.at(1) == String("b"));
    EXPECT(test_string_vector.at(2) == String("c"));
    EXPECT(test_string_vector.at(3) == String("d"));

    test_string = "```\nHello there\r\nHello there\n```";
    test_string_view = test_string.view();
    test_string_vector = test_string_view.lines();
    EXPECT_EQ(test_string_vector.size(), 4u);
    EXPECT(test_string_vector.at(0) == String("```"));
    EXPECT(test_string_vector.at(1) == String("Hello there"));
    EXPECT(test_string_vector.at(2) == String("Hello there"));
    EXPECT(test_string_vector.at(3) == String("```"));

    test_string = "\n\n\n";
    test_string_view = test_string.view();
    test_string_vector = test_string_view.lines();
    EXPECT_EQ(test_string_vector.size(), 3u);
    EXPECT_EQ(test_string_vector.at(0).is_empty(), true);
    EXPECT_EQ(test_string_vector.at(1).is_empty(), true);
    EXPECT_EQ(test_string_vector.at(2).is_empty(), true);
}

TEST_CASE(find_first_of)
{
    String test_string = "aabbcc_xy_ccbbaa";
    StringView test_string_view = test_string.view();

    EXPECT_EQ(test_string_view.find_first_of('b').has_value(), true);
    EXPECT_EQ(test_string_view.find_first_of('b').value(), 2U);

    EXPECT_EQ(test_string_view.find_first_of('_').has_value(), true);
    EXPECT_EQ(test_string_view.find_first_of('_').value(), 6U);

    EXPECT_EQ(test_string_view.find_first_of("bc").has_value(), true);
    EXPECT_EQ(test_string_view.find_first_of("bc").value(), 2U);

    EXPECT_EQ(test_string_view.find_first_of("yx").has_value(), true);
    EXPECT_EQ(test_string_view.find_first_of("yx").value(), 7U);

    EXPECT_EQ(test_string_view.find_first_of('n').has_value(), false);
    EXPECT_EQ(test_string_view.find_first_of("defg").has_value(), false);
}

TEST_CASE(find_last_of)
{
    String test_string = "aabbcc_xy_ccbbaa";
    StringView test_string_view = test_string.view();

    EXPECT_EQ(test_string_view.find_last_of('b').has_value(), true);
    EXPECT_EQ(test_string_view.find_last_of('b').value(), 13U);

    EXPECT_EQ(test_string_view.find_last_of('_').has_value(), true);
    EXPECT_EQ(test_string_view.find_last_of('_').value(), 9U);

    EXPECT_EQ(test_string_view.find_last_of("bc").has_value(), true);
    EXPECT_EQ(test_string_view.find_last_of("bc").value(), 13U);

    EXPECT_EQ(test_string_view.find_last_of("yx").has_value(), true);
    EXPECT_EQ(test_string_view.find_last_of("yx").value(), 8U);

    EXPECT_EQ(test_string_view.find_last_of('3').has_value(), false);
    EXPECT_EQ(test_string_view.find_last_of("fghi").has_value(), false);

    test_string_view = "/";
    EXPECT_EQ(test_string_view.find_last_of('/').has_value(), true);
    EXPECT_EQ(test_string_view.find_last_of('/').value(), 0U);
    EXPECT_EQ(test_string_view.find_last_of("/").has_value(), true);
    EXPECT_EQ(test_string_view.find_last_of("/").value(), 0U);
}

TEST_CASE(split_view)
{
    StringView test_string_view = "axxbxcxd";
    EXPECT_EQ(test_string_view.split_view('x'), Vector<StringView>({ "a", "b", "c", "d" }));
    EXPECT_EQ(test_string_view.split_view('x', true), Vector<StringView>({ "a", "", "b", "c", "d" }));
    EXPECT_EQ(test_string_view.split_view("x"), Vector<StringView>({ "a", "b", "c", "d" }));
    EXPECT_EQ(test_string_view.split_view("x", true), Vector<StringView>({ "a", "", "b", "c", "d" }));

    test_string_view = "axxbx";
    EXPECT_EQ(test_string_view.split_view('x'), Vector<StringView>({ "a", "b" }));
    EXPECT_EQ(test_string_view.split_view('x', true), Vector<StringView>({ "a", "", "b", "" }));
    EXPECT_EQ(test_string_view.split_view("x"), Vector<StringView>({ "a", "b" }));
    EXPECT_EQ(test_string_view.split_view("x", true), Vector<StringView>({ "a", "", "b", "" }));

    test_string_view = "axxbcxxdxx";
    EXPECT_EQ(test_string_view.split_view("xx"), Vector<StringView>({ "a", "bc", "d" }));
    EXPECT_EQ(test_string_view.split_view("xx", true), Vector<StringView>({ "a", "bc", "d", "" }));

    test_string_view = "ax_b_cxd";
    auto predicate = [](char ch) { return ch == 'x' || ch == '_'; };
    EXPECT_EQ(test_string_view.split_view_if(predicate), Vector<StringView>({ "a", "b", "c", "d" }));
    EXPECT_EQ(test_string_view.split_view_if(predicate, true), Vector<StringView>({ "a", "", "b", "c", "d" }));
    EXPECT_EQ(test_string_view.split_view_if(predicate), Vector<StringView>({ "a", "b", "c", "d" }));
    EXPECT_EQ(test_string_view.split_view_if(predicate, true), Vector<StringView>({ "a", "", "b", "c", "d" }));
}
