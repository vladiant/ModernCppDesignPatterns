/// \file builder.hpp
/// \brief Builder (C4) — *deducing this* fluent setters (one body serves `&`
///        and `&&`) and a validated `build()` returning `std::expected`.

#ifndef MCPP_CREATIONAL26_BUILDER_HPP
#define MCPP_CREATIONAL26_BUILDER_HPP

#include <array>
#include <algorithm>
#include <expected>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace mcpp::creational26 {

/// Why a `build()` failed — reported, never thrown.
enum class BuildError {
    missing_url,        ///< url was never set / is empty.
    invalid_method,     ///< method not in the allowed verb set.
    empty_header_name,  ///< a header was added with an empty name.
};

/// The immutable product assembled by `HttpRequestBuilder`.
struct HttpRequest {
    std::string method;
    std::string url;
    std::vector<std::pair<std::string, std::string>> headers;
    std::string body;

    bool operator==(const HttpRequest&) const = default;
};

/// Fluent builder whose setters use *deducing this*: a single
/// `template <class Self> auto&& with_x(this Self&& self, ...)` body serves both
/// the lvalue (`&`) and rvalue (`&&`) call forms and preserves the caller's
/// value category, so an rvalue chain can move its fields straight into the
/// product.
class HttpRequestBuilder {
public:
    /// Set the HTTP method (defaults to "GET").
    template <class Self>
    auto&& with_method(this Self&& self, std::string method) {
        self.method_ = std::move(method);
        return std::forward<Self>(self);
    }

    /// Set the request URL (required).
    template <class Self>
    auto&& with_url(this Self&& self, std::string url) {
        self.url_ = std::move(url);
        return std::forward<Self>(self);
    }

    /// Append a header. An empty \p name makes `build()` fail.
    template <class Self>
    auto&& with_header(this Self&& self, std::string name, std::string value) {
        self.headers_.emplace_back(std::move(name), std::move(value));
        return std::forward<Self>(self);
    }

    /// Set the request body.
    template <class Self>
    auto&& with_body(this Self&& self, std::string body) {
        self.body_ = std::move(body);
        return std::forward<Self>(self);
    }

    /// Validate and assemble from a const lvalue builder (fields are copied).
    [[nodiscard]] std::expected<HttpRequest, BuildError> build(
        this const HttpRequestBuilder& self) {
        if (const auto err = self.validate()) {
            return std::unexpected(*err);
        }
        return HttpRequest{self.method_, self.url_, self.headers_, self.body_};
    }

    /// Validate and assemble from an rvalue builder (fields are moved out).
    [[nodiscard]] std::expected<HttpRequest, BuildError> build(
        this HttpRequestBuilder&& self) {
        if (const auto err = self.validate()) {
            return std::unexpected(*err);
        }
        return HttpRequest{std::move(self.method_), std::move(self.url_),
                           std::move(self.headers_), std::move(self.body_)};
    }

private:
    [[nodiscard]] std::optional<BuildError> validate() const {
        static constexpr std::array<std::string_view, 5> allowed{
            "GET", "POST", "PUT", "DELETE", "PATCH"};
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

}  // namespace mcpp::creational26

#endif  // MCPP_CREATIONAL26_BUILDER_HPP
