/// \file main.cpp
/// \brief Builder demo: deducing-this fluent chain + std::expected validation.

#include <mcpp/builder/request_builder.hpp>

#include <format>
#include <iostream>
#include <string>

namespace {
using namespace mcpp::builder;

std::string describe(const BuildError& e) {
    switch (e) {
    case BuildError::missing_url:
        return "missing_url";
    case BuildError::invalid_method:
        return "invalid_method";
    case BuildError::empty_header_name:
        return "empty_header_name";
    }
    return "unknown";
}

void print(const std::expected<HttpRequest, BuildError>& result) {
    if (result) {
        std::cout << std::format("OK: {} {} (headers={}, body='{}')\n", result->method,
                                 result->url, result->headers.size(), result->body);
    } else {
        std::cout << std::format("ERR: {}\n", describe(result.error()));
    }
}
} // namespace

int main() {
    using namespace mcpp::builder;

    // Valid request via a fluent lvalue chain.
    RequestBuilder b;
    b.method("POST").url("https://example.com/api").header("Accept", "application/json").body("{}");
    print(b.build());

    // Invalid: missing url.
    print(RequestBuilder{}.method("GET").build());

    // Invalid: unsupported method.
    print(RequestBuilder{}.method("TELEPORT").url("https://example.com").build());

    // Rvalue chain builds from a temporary and moves fields out.
    print(RequestBuilder{}.method("PUT").url("https://example.com/item/1").build());

    return 0;
}
