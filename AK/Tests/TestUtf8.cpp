

#include <LibTest/TestCase.h>

#include <AK/Utf8View.h>

TEST_CASE(decode_ascii)
{
    Utf8View utf8 { "Hello World!11" };
    EXPECT(utf8.validate());

    u32 expected[] = { 72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 33, 49, 49 };
    size_t expected_size = sizeof(expected) / sizeof(expected[0]);

    size_t i = 0;
    for (u32 code_point : utf8) {
        VERIFY(i < expected_size);
        EXPECT_EQ(code_point, expected[i]);
        i++;
    }
    EXPECT_EQ(i, expected_size);
}

TEST_CASE(decode_utf8)
{
    Utf8View utf8 { "Привет, мир! 😀 γειά σου κόσμος こんにちは世界" };
    size_t valid_bytes;
    EXPECT(utf8.validate(valid_bytes));
    EXPECT(valid_bytes == (size_t)utf8.byte_length());

    u32 expected[] = { 1055, 1088, 1080, 1074, 1077, 1090, 44, 32, 1084, 1080, 1088, 33, 32, 128512, 32, 947, 949, 953, 940, 32, 963, 959, 965, 32, 954, 972, 963, 956, 959, 962, 32, 12371, 12435, 12395, 12385, 12399, 19990, 30028 };
    size_t expected_size = sizeof(expected) / sizeof(expected[0]);

    size_t i = 0;
    for (u32 code_point : utf8) {
        VERIFY(i < expected_size);
        EXPECT_EQ(code_point, expected[i]);
        i++;
    }
    EXPECT_EQ(i, expected_size);
}

TEST_CASE(validate_invalid_ut8)
{
    size_t valid_bytes;
    char invalid_utf8_1[] = { 42, 35, (char)182, 9, 0 };
    Utf8View utf8_1 { invalid_utf8_1 };
    EXPECT(!utf8_1.validate(valid_bytes));
    EXPECT(valid_bytes == 2);

    char invalid_utf8_2[] = { 42, 35, (char)208, (char)208, 0 };
    Utf8View utf8_2 { invalid_utf8_2 };
    EXPECT(!utf8_2.validate(valid_bytes));
    EXPECT(valid_bytes == 2);

    char invalid_utf8_3[] = { (char)208, 0 };
    Utf8View utf8_3 { invalid_utf8_3 };
    EXPECT(!utf8_3.validate(valid_bytes));
    EXPECT(valid_bytes == 0);

    char invalid_utf8_4[] = { (char)208, 35, 0 };
    Utf8View utf8_4 { invalid_utf8_4 };
    EXPECT(!utf8_4.validate(valid_bytes));
    EXPECT(valid_bytes == 0);
}
