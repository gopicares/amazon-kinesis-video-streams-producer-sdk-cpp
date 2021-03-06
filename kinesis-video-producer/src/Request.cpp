#include "Request.h"
#include "CurlCallManager.h"

#include <cassert>

using namespace std;

namespace com { namespace amazonaws { namespace kinesis { namespace video {

Request::Request(Verb verb, const string &url)
        : creation_time_(std::chrono::system_clock::now()),
          verb_(verb),
          url_(url),
          request_completion_timeout_(std::chrono::duration<double, std::milli>::zero()),
          connection_timeout_(std::chrono::duration<double, std::milli>::zero()),
          is_streaming_(false) {
}

Request::Request(Request::Verb verb,
                 const std::string &url,
                 CurlHeaderCallbackFn post_header_callback,
                 CurlReadCallbackFn post_callback,
                 void *post_read_callback_custom_data,
                 CurlWriteCallbackFn post_write_callback,
                 void *post_write_callback_custom_data)
        : creation_time_(std::chrono::system_clock::now()),
          verb_(verb),
          url_(url),
          request_completion_timeout_(std::chrono::duration<double, std::milli>::zero()),
          connection_timeout_(std::chrono::duration<double, std::milli>::zero()),
          is_streaming_(true),
          post_header_callback_(post_header_callback),
          post_read_callback_(post_callback),
          post_read_callback_custom_data_(post_read_callback_custom_data),
          post_write_callback_(post_write_callback),
          post_write_callback_custom_data_(post_write_callback_custom_data) {
    assert(post_read_callback_custom_data_);
}

Request::~Request() {
}

void Request::setBody(const void *body, size_t input_size) {
    body_.assign(reinterpret_cast<const char *>(body), input_size);
}

void Request::setBody(const string &body) {
    body_ = body;
}

void Request::setHeader(const string &header_name, const string &header_value) {
    headers_[header_name] = header_value;
}

void Request::setRequestCompletionTimeout(std::chrono::duration<double, std::milli> timeout) {
    request_completion_timeout_ = timeout;
}

void Request::setConnectionTimeout(std::chrono::duration<double, std::milli> timeout) {
    connection_timeout_ = timeout;
}

void Request::setUrl(const std::string &url) {
    url_ = url;
}

void Request::setVerb(Verb verb) {
    verb_ = verb;
}

const string& Request::getBody() const {
    return body_;
}

const chrono::system_clock::time_point Request::getCreationTime() const {
    return creation_time_;
}

const string *Request::getHeader(const string &header) const {
    HeaderMap::const_iterator i = headers_.find(header);
    if (headers_.end() == i) {
        return NULL;
    }
    return &i->second;
}

const Request::HeaderMap &Request::getHeaders() const {
    return headers_;
}

const std::chrono::duration<double, std::milli> Request::getRequestCompletionTimeout() const {
    return request_completion_timeout_;
}

const std::chrono::duration<double, std::milli> Request::getConnectionTimeout() const {
    return connection_timeout_;
}

const string &Request::get_url() const {
    return url_;
}

Request::Verb Request::getVerb() const {
    return verb_;
}

string Request::getScheme() const {
    const string &url = get_url();
    size_t scheme_delim = url.find("://");
    if (scheme_delim == string::npos) {
        throw runtime_error("unable to find URI scheme delimiter");
    }
    return url.substr(0, scheme_delim);
}

string Request::getHost() const {
    const string &url = get_url();

    // find the start of the host name
    size_t begin_delim = url.find("://");
    if (begin_delim == string::npos) {
        throw runtime_error("unable to find URI scheme delimiter");
    }
    begin_delim += 3;

    // find the end of the host name (end at path delimiter port or query string)
    size_t end_delim = url.find_first_of("/:?", begin_delim);

    // return the host name
    return url.substr(begin_delim, end_delim - begin_delim);
}

string Request::getPath() const {
    const string &url = get_url();
    // find the start of the path
    size_t begin_delim = url.find("://");
    if (begin_delim == string::npos) {
        throw runtime_error("unable to find URI scheme delimiter");
    }
    begin_delim = url.find('/', begin_delim + 3);
    if (begin_delim == string::npos) {
        return string();
    }
    // find the end of the path
    size_t end_delim = url.find('?', begin_delim);
    // return the path
    return url.substr(begin_delim, end_delim - begin_delim);
}

string Request::getQuery() const {
    const string &url = get_url();
    size_t query_delim = url.find('?');
    if (query_delim == string::npos) {
        return string();
    }
    return url.substr(query_delim + 1);
}

bool Request::isStreaming() const {
    return is_streaming_;
}

Request::CurlReadCallbackFn Request::getPostReadCallback() const {
    return post_read_callback_;
}

Request::CurlHeaderCallbackFn Request::getPostHeaderCallback() const {
    return post_header_callback_;
}

void *Request::getPostReadCallbackCustomData() const {
    return post_read_callback_custom_data_;
}

Request::CurlWriteCallbackFn Request::getPostWriteCallback() const {
    return post_write_callback_;
}

void *Request::getPostWriteCallbackCustomData() const {
    return post_write_callback_custom_data_;
}

} // namespace video
} // namespace kinesis
} // namespace amazonaws
} // namespace com
