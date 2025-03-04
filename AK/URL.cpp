
#include <AK/LexicalPath.h>
#include <AK/StringBuilder.h>
#include <AK/URL.h>
#include <AK/URLParser.h>

namespace AK {

static inline bool is_valid_protocol_character(char ch)
{
    return ch >= 'a' && ch <= 'z';
}

static inline bool is_valid_hostname_character(char ch)
{
    return ch && ch != '/' && ch != ':';
}

static inline bool is_digit(char ch)
{
    return ch >= '0' && ch <= '9';
}

bool URL::parse(const StringView& string)
{
    if (string.is_null())
        return false;

    enum class State {
        InProtocol,
        InHostname,
        InPort,
        InPath,
        InQuery,
        InFragment,
        InDataMimeType,
        InDataPayload,
    };

    Vector<char, 256> buffer;
    State state { State::InProtocol };

    size_t index = 0;

    auto peek = [&] {
        if (index >= string.length())
            return '\0';
        return string[index];
    };

    auto consume = [&] {
        if (index >= string.length())
            return '\0';
        return string[index++];
    };

    while (index < string.length()) {
        switch (state) {
        case State::InProtocol: {
            if (is_valid_protocol_character(peek())) {
                buffer.append(consume());
                continue;
            }
            if (consume() != ':')
                return false;

            m_protocol = String::copy(buffer);

            if (m_protocol == "data") {
                buffer.clear();
                m_host = "";
                state = State::InDataMimeType;
                continue;
            }

            if (m_protocol == "about") {
                buffer.clear();
                m_host = "";
                state = State::InPath;
                continue;
            }

            if (consume() != '/')
                return false;
            if (consume() != '/')
                return false;
            if (buffer.is_empty())
                return false;
            state = State::InHostname;
            buffer.clear();
            continue;
        }
        case State::InHostname:
            if (is_valid_hostname_character(peek())) {
                buffer.append(consume());
                continue;
            }
            if (buffer.is_empty()) {
                if (m_protocol == "file") {
                    m_host = "";
                    state = State::InPath;
                    continue;
                }
                return false;
            }
            m_host = String::copy(buffer);
            buffer.clear();
            if (peek() == ':') {
                consume();
                state = State::InPort;
                continue;
            }
            if (peek() == '/') {
                state = State::InPath;
                continue;
            }
            return false;
        case State::InPort:
            if (is_digit(peek())) {
                buffer.append(consume());
                continue;
            }
            if (buffer.is_empty())
                return false;
            {
                auto port_opt = String::copy(buffer).to_uint();
                buffer.clear();
                if (!port_opt.has_value())
                    return false;
                m_port = port_opt.value();
            }
            if (peek() == '/') {
                state = State::InPath;
                continue;
            }
            return false;
        case State::InPath:
            if (peek() == '?' || peek() == '#') {
                m_path = String::copy(buffer);
                buffer.clear();
                state = peek() == '?' ? State::InQuery : State::InFragment;
                consume();
                continue;
            }
            buffer.append(consume());
            continue;
        case State::InQuery:
            if (peek() == '#') {
                m_query = String::copy(buffer);
                buffer.clear();
                consume();
                state = State::InFragment;
                continue;
            }
            buffer.append(consume());
            continue;
        case State::InFragment:
            buffer.append(consume());
            continue;
        case State::InDataMimeType: {
            if (peek() != ';' && peek() != ',') {
                buffer.append(consume());
                continue;
            }

            m_data_mime_type = String::copy(buffer);
            buffer.clear();

            if (peek() == ';') {
                consume();
                if (consume() != 'b')
                    return false;
                if (consume() != 'a')
                    return false;
                if (consume() != 's')
                    return false;
                if (consume() != 'e')
                    return false;
                if (consume() != '6')
                    return false;
                if (consume() != '4')
                    return false;
                m_data_payload_is_base64 = true;
            }

            if (consume() != ',')
                return false;

            state = State::InDataPayload;
            break;
        }
        case State::InDataPayload:
            buffer.append(consume());
            break;
        }
    }
    if (state == State::InHostname) {
        // We're still in the hostname, so e.g "http://pranaosos.org"
        if (buffer.is_empty())
            return false;
        m_host = String::copy(buffer);
        m_path = "/";
    }
    if (state == State::InProtocol)
        return false;
    if (state == State::InPath)
        m_path = String::copy(buffer);
    if (state == State::InQuery)
        m_query = String::copy(buffer);
    if (state == State::InFragment)
        m_fragment = String::copy(buffer);
    if (state == State::InDataPayload)
        m_data_payload = urldecode(String::copy(buffer));
    if (state == State::InPort) {
        auto port_opt = String::copy(buffer).to_uint();
        if (port_opt.has_value())
            m_port = port_opt.value();
    }

    if (m_query.is_null())
        m_query = "";
    if (m_fragment.is_null())
        m_fragment = "";

    if (!m_port && protocol_requires_port(m_protocol))
        set_port(default_port_for_protocol(m_protocol));

    return compute_validity();
}

URL::URL(const StringView& string)
{
    m_valid = parse(string);
}

String URL::to_string() const
{
    StringBuilder builder;
    builder.append(m_protocol);

    if (m_protocol == "about") {
        builder.append(':');
        builder.append(m_path);
        return builder.to_string();
    }

    if (m_protocol == "data") {
        builder.append(':');
        builder.append(m_data_mime_type);
        if (m_data_payload_is_base64)
            builder.append(";base64");
        builder.append(',');
        builder.append(m_data_payload);
        return builder.to_string();
    }

    builder.append("://");
    builder.append(m_host);
    if (default_port_for_protocol(protocol()) != port()) {
        builder.append(':');
        builder.append(String::number(m_port));
    }

    builder.append(m_path);
    if (!m_query.is_empty()) {
        builder.append('?');
        builder.append(m_query);
    }
    if (!m_fragment.is_empty()) {
        builder.append('#');
        builder.append(m_fragment);
    }
    return builder.to_string();
}

URL URL::complete_url(const String& string) const
{
    if (!is_valid())
        return {};

    URL url(string);
    if (url.is_valid())
        return url;

    if (protocol() == "data")
        return {};

    if (string.starts_with("//")) {
        URL url(String::formatted("{}:{}", m_protocol, string));
        if (url.is_valid())
            return url;
    }

    if (string.starts_with("/")) {
        url = *this;
        url.set_path(string);
        return url;
    }

    if (string.starts_with("#")) {
        url = *this;
        url.set_fragment(string.substring(1, string.length() - 1));
        return url;
    }

    StringBuilder builder;
    LexicalPath lexical_path(path());
    builder.append('/');

    bool document_url_ends_in_slash = path()[path().length() - 1] == '/';

    for (size_t i = 0; i < lexical_path.parts().size(); ++i) {
        if (i == lexical_path.parts().size() - 1 && !document_url_ends_in_slash)
            break;
        builder.append(lexical_path.parts()[i]);
        builder.append('/');
    }
    builder.append(string);
    auto built = builder.to_string();
    lexical_path = LexicalPath(built);

    built = lexical_path.string();
    if (string.ends_with('/') && !built.ends_with('/')) {
        builder.clear();
        builder.append(built);
        builder.append('/');
        built = builder.to_string();
    }

    url = *this;
    url.set_path(built);
    return url;
}

void URL::set_protocol(const String& protocol)
{
    m_protocol = protocol;
    m_valid = compute_validity();
}

void URL::set_host(const String& host)
{
    m_host = host;
    m_valid = compute_validity();
}

void URL::set_port(u16 port)
{
    m_port = port;
    m_valid = compute_validity();
}

void URL::set_path(const String& path)
{
    m_path = path;
    m_valid = compute_validity();
}

void URL::set_query(const String& query)
{
    m_query = query;
}

void URL::set_fragment(const String& fragment)
{
    m_fragment = fragment;
}

bool URL::compute_validity() const
{
    // FIXME: This is by no means complete.
    if (m_protocol.is_empty())
        return false;

    if (m_protocol == "about") {
        if (m_path.is_empty())
            return false;
        return true;
    }

    if (m_protocol == "file") {
        if (m_path.is_empty())
            return false;
        return true;
    }

    if (m_protocol == "data") {
        if (m_data_mime_type.is_empty())
            return false;
        return true;
    }

    if (m_host.is_empty())
        return false;

    if (!m_port && protocol_requires_port(m_protocol))
        return false;

    return true;
}

bool URL::protocol_requires_port(const String& protocol)
{
    return (default_port_for_protocol(protocol) != 0);
}

u16 URL::default_port_for_protocol(const String& protocol)
{
    if (protocol == "http")
        return 80;
    if (protocol == "https")
        return 443;
    if (protocol == "gemini")
        return 1965;
    if (protocol == "irc")
        return 6667;
    if (protocol == "ircs")
        return 6697;
    if (protocol == "ws")
        return 80;
    if (protocol == "wss")
        return 443;
    return 0;
}

URL URL::create_with_file_protocol(const String& path, const String& fragment)
{
    URL url;
    url.set_protocol("file");
    url.set_path(path);
    url.set_fragment(fragment);
    return url;
}

URL URL::create_with_url_or_path(const String& url_or_path)
{
    URL url = url_or_path;
    if (url.is_valid())
        return url;

    String path = LexicalPath::canonicalized_path(url_or_path);
    return URL::create_with_file_protocol(path);
}

URL URL::create_with_data(const StringView& mime_type, const StringView& payload, bool is_base64)
{
    URL url;
    url.set_protocol("data");
    url.m_valid = true;
    url.m_data_payload = payload;
    url.m_data_mime_type = mime_type;
    url.m_data_payload_is_base64 = is_base64;

    return url;
}

String URL::basename() const
{
    if (!m_valid)
        return {};
    return LexicalPath(m_path).basename();
}

}
