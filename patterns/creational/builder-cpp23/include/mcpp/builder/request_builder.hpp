#ifndef MCPP_BUILDER_REQUEST_BUILDER_HPP
#define MCPP_BUILDER_REQUEST_BUILDER_HPP

/// \file request_builder.hpp
/// \brief Builder pattern for an immutable HttpRequest (C++23).
///
/// Showcased idioms:
///  - **deducing this** (explicit object parameter): the fluent setters return
///    the builder with the caller's own value category preserved, so chaining
///    on an rvalue yields rvalues (move-out) and on an lvalue yields lvalue
///    refs.
///  - **`std::expected<T, E>`**: a validated terminal `build()`.

#include <algorithm>
#include <array>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mcpp::builder {

/// Why a `build()` failed.
enum class BuildError { missing_url, invalid_method, empty_header_name };

/// The immutable product.
struct HttpRequest {
    std::string method;
    std::string url;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;
};

/// Fluent builder. Setters use deducing this to preserve value category.
class RequestBuilder {
public:
    /// Set the HTTP method (defaults to "GET").
    template <class Self>
    auto&& method(this Self&& self, std::string m) {
        self.method_ = std::move(m);
        return std::forward<Self>(self);
    }

    /// Set the request URL (required).
    template <class Self>
    auto&& url(this Self&& self, std::string u) {
        self.url_ = std::move(u);
        return std::forward<Self>(self);
    }

    /// Append a header. An empty \p name causes `build()` to fail.
    template <class Self>
    auto&& header(this Self&& self, std::string name, std::string value) {
        self.headers_.emplace_back(std::move(name), std::move(value));
        return std::forward<Self>(self);
    }

    /// Set the request body.
    template <class Self>
    auto&& body(this Self&& self, std::string b) {
        self.body_ = std::move(b);
        return std::forward<Self>(self);
    }

    /// Validate and assemble (copying fields). Value category: const lvalue.
    [[nodiscard]] std::expected<HttpRequest, BuildError> build(this const RequestBuilder& self) {
        if (auto err = self.validate()) {
            return std::unexpected(*err);
        }
        return HttpRequest{self.method_, self.url_, self.headers_, self.body_};
    }

    /// Validate and assemble, moving fields out of an rvalue builder.
    [[nodiscard]] std::expected<HttpRequest, BuildError> build(this RequestBuilder&& self) {
        if (auto err = self.validate()) {
            return std::unexpected(*err);
        }
        return HttpRequest{std::move(self.method_), std::move(self.url_),
                           std::move(self.headers_), std::move(self.body_)};
    }

private:
    /// Shared validation. Returns the first error, or `std::nullopt` if valid.
    [[nodiscard]] std::optional<BuildError> validate() const {
        static constexpr std::array<std::string_view, 5> allowed{"GET", "POST", "PUT", "DELETE",
                                                                  "PATCH"};
        if (url_.empty()) {
            return BuildError::missing_url;
        }
        if (std::ranges::find(allowed, method_) == allowed.end()) {
            return BuildError::invalid_method;
        }
        for (const auto& [name, value] : headers_) {
            if (name.empty()) {
                return BuildError::empty_header_name;
            }
        }
        return std::nullopt;
    }

    std::string method_{"GET"};
    std::string url_;
    std::vector<std::pair<std::string, std::string>> headers_;
    std::string body_;
};

} // namespace mcpp::builder

#endif // MCPP_BUILDER_REQUEST_BUILDER_HPP
