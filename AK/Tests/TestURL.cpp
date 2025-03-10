
#include <LibTest/TestCase.h>

#include <AK/URL.h>

TEST_CASE(construct)
{
    EXPECT_EQ(URL().is_valid(), false);
}

TEST_CASE(basic)
{
    {
        URL url("http://www.pranaosos.org");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "http");
        EXPECT_EQ(url.host(), "www.pranaosos.org");
        EXPECT_EQ(url.port(), 80);
        EXPECT_EQ(url.path(), "/");
        EXPECT_EQ(url.query(), "");
        EXPECT_EQ(url.fragment(), "");
    }
    {
        URL url("https://www.pranaosos.org/index.html");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "https");
        EXPECT_EQ(url.host(), "www.pranaosos.org");
        EXPECT_EQ(url.port(), 443);
        EXPECT_EQ(url.path(), "/index.html");
        EXPECT_EQ(url.query(), "");
        EXPECT_EQ(url.fragment(), "");
    }
    {
        URL url("https://localhost:1234/~anon/test/page.html");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "https");
        EXPECT_EQ(url.host(), "localhost");
        EXPECT_EQ(url.port(), 1234);
        EXPECT_EQ(url.path(), "/~anon/test/page.html");
        EXPECT_EQ(url.query(), "");
        EXPECT_EQ(url.fragment(), "");
    }
    {
        URL url("http://www.pranaosos.org/index.html?#");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "http");
        EXPECT_EQ(url.host(), "www.pranaosos.org");
        EXPECT_EQ(url.port(), 80);
        EXPECT_EQ(url.path(), "/index.html");
        EXPECT_EQ(url.query(), "");
        EXPECT_EQ(url.fragment(), "");
    }
    {
        URL url("http://www.pranaosos.org/index.html?foo=1&bar=2");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "http");
        EXPECT_EQ(url.host(), "www.pranaosos.org");
        EXPECT_EQ(url.port(), 80);
        EXPECT_EQ(url.path(), "/index.html");
        EXPECT_EQ(url.query(), "foo=1&bar=2");
        EXPECT_EQ(url.fragment(), "");
    }
    {
        URL url("http://www.pranaosos.org/index.html#fragment");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "http");
        EXPECT_EQ(url.host(), "www.pranaosos.org");
        EXPECT_EQ(url.port(), 80);
        EXPECT_EQ(url.path(), "/index.html");
        EXPECT_EQ(url.query(), "");
        EXPECT_EQ(url.fragment(), "fragment");
    }
    {
        URL url("http://www.pranaosos.org/index.html?foo=1&bar=2&baz=/?#frag/ment?test#");
        EXPECT_EQ(url.is_valid(), true);
        EXPECT_EQ(url.protocol(), "http");
        EXPECT_EQ(url.host(), "www.pranaosos.org");
        EXPECT_EQ(url.port(), 80);
        EXPECT_EQ(url.path(), "/index.html");
        EXPECT_EQ(url.query(), "foo=1&bar=2&baz=/?");
        EXPECT_EQ(url.fragment(), "frag/ment?test#");
    }
}

TEST_CASE(some_bad_urls)
{
    EXPECT_EQ(URL("http:pranaosos.org").is_valid(), false);
    EXPECT_EQ(URL("http:/pranaosos.org").is_valid(), false);
    EXPECT_EQ(URL("http//pranaosos.org").is_valid(), false);
    EXPECT_EQ(URL("http:///pranaosos.org").is_valid(), false);
    EXPECT_EQ(URL("pranaosos.org").is_valid(), false);
    EXPECT_EQ(URL("://pranaosos.org").is_valid(), false);
    EXPECT_EQ(URL("://:80").is_valid(), false);
    EXPECT_EQ(URL("http://pranaosos.org:80:80/").is_valid(), false);
    EXPECT_EQ(URL("http://pranaosos.org:80:80").is_valid(), false);
    EXPECT_EQ(URL("http://pranaosos.org:abc").is_valid(), false);
    EXPECT_EQ(URL("http://pranaosos.org:abc:80").is_valid(), false);
    EXPECT_EQ(URL("http://pranaosos.org:abc:80/").is_valid(), false);
    EXPECT_EQ(URL("http://pranaosos.org:/abc/").is_valid(), false);
    EXPECT_EQ(URL("data:").is_valid(), false);
    EXPECT_EQ(URL("file:").is_valid(), false);
    EXPECT_EQ(URL("about:").is_valid(), false);
}

TEST_CASE(serialization)
{
    EXPECT_EQ(URL("http://www.pranaosos.org/").to_string(), "http://www.pranaosos.org/");
    EXPECT_EQ(URL("http://www.pranaosos.org:0/").to_string(), "http://www.pranaosos.org/");
    EXPECT_EQ(URL("http://www.pranaosos.org:80/").to_string(), "http://www.pranaosos.org/");
    EXPECT_EQ(URL("http://www.pranaosos.org:81/").to_string(), "http://www.pranaosos.org:81/");
    EXPECT_EQ(URL("https://www.pranaosos.org:443/foo/bar.html?query#fragment").to_string(), "https://www.pranaosos.org/foo/bar.html?query#fragment");
}

TEST_CASE(file_url_with_hostname)
{
    URL url("file://localhost/my/file");
    EXPECT_EQ(url.is_valid(), true);
    EXPECT_EQ(url.host(), "localhost");
    EXPECT_EQ(url.path(), "/my/file");
    EXPECT_EQ(url.to_string(), "file://localhost/my/file");
}

TEST_CASE(file_url_without_hostname)
{
    URL url("file:///my/file");
    EXPECT_EQ(url.is_valid(), true);
    EXPECT_EQ(url.protocol(), "file");
    EXPECT_EQ(url.host(), "");
    EXPECT_EQ(url.path(), "/my/file");
    EXPECT_EQ(url.to_string(), "file:///my/file");
}

TEST_CASE(about_url)
{
    URL url("about:blank");
    EXPECT_EQ(url.is_valid(), true);
    EXPECT_EQ(url.protocol(), "about");
    EXPECT_EQ(url.host(), "");
    EXPECT_EQ(url.path(), "blank");
    EXPECT_EQ(url.to_string(), "about:blank");
}

TEST_CASE(data_url)
{
    URL url("data:text/html,test");
    EXPECT_EQ(url.is_valid(), true);
    EXPECT_EQ(url.protocol(), "data");
    EXPECT_EQ(url.host(), "");
    EXPECT_EQ(url.data_mime_type(), "text/html");
    EXPECT_EQ(url.data_payload(), "test");
    EXPECT_EQ(url.to_string(), "data:text/html,test");
}

TEST_CASE(data_url_encoded)
{
    URL url("data:text/html,Hello%20friends%2C%0X%X0");
    EXPECT_EQ(url.is_valid(), true);
    EXPECT_EQ(url.protocol(), "data");
    EXPECT_EQ(url.host(), "");
    EXPECT_EQ(url.data_mime_type(), "text/html");
    EXPECT_EQ(url.data_payload(), "Hello friends,%0X%X0");
    // FIXME: Surely this should be URL-encoded again?!
    EXPECT_EQ(url.to_string(), "data:text/html,Hello friends,%0X%X0");
}

TEST_CASE(data_url_base64_encoded)
{
    URL url("data:text/html;base64,test");
    EXPECT_EQ(url.is_valid(), true);
    EXPECT_EQ(url.protocol(), "data");
    EXPECT_EQ(url.host(), "");
    EXPECT_EQ(url.data_mime_type(), "text/html");
    EXPECT_EQ(url.data_payload(), "test");
    EXPECT_EQ(url.to_string(), "data:text/html;base64,test");
}

TEST_CASE(trailing_slash_with_complete_url)
{
    EXPECT_EQ(URL("http://a/b/").complete_url("c/").to_string(), "http://a/b/c/");
    EXPECT_EQ(URL("http://a/b/").complete_url("c").to_string(), "http://a/b/c");
    EXPECT_EQ(URL("http://a/b").complete_url("c/").to_string(), "http://a/c/");
    EXPECT_EQ(URL("http://a/b").complete_url("c").to_string(), "http://a/c");
}

TEST_CASE(trailing_port)
{
    URL url("http://example.com:8086");
    EXPECT_EQ(url.port(), 8086);
}

TEST_CASE(port_int_overflow_wrap)
{
    auto expected_port = 80;
    URL url(String::formatted("http://example.com:{}/", (u32)((65536 * 1000) + expected_port)));
    EXPECT_EQ(url.port(), expected_port);
    EXPECT_EQ(url.is_valid(), true);
}
