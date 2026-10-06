/// \file proxy.hpp
/// \brief Proxy (C12) — a virtual proxy for an expensive `Image`. The real
///        subject is constructed exactly once, lazily, via `std::call_once`
///        + `std::once_flag` (replacing a hand-rolled bool flag + mutex).

#ifndef MCPP_STRUCTURAL26_PROXY_HPP
#define MCPP_STRUCTURAL26_PROXY_HPP

#include "compat.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <utility>

namespace mcpp::structural26 {

/// Abstract subject: something that can be drawn.
class Image {
public:
    virtual ~Image() = default;
    [[nodiscard]] virtual std::string draw() const = 0;
};

/// The expensive real subject. Constructing it is the "costly load"; we count
/// how many times that happens to prove the proxy loads it at most once.
class RealImage final : public Image {
public:
    explicit RealImage(std::string path) : path_(std::move(path)) {
        ++load_count_;
    }

    [[nodiscard]] std::string draw() const override {
        return "<image " + path_ + ">";
    }

    /// Process-wide count of real-image loads (test/demo observability).
    static int load_count() { return load_count_; }
    static void reset_load_count() { load_count_ = 0; }

private:
    std::string path_;
    inline static int load_count_ = 0;
};

/// Virtual proxy: defers construction of the `RealImage` until first use, and
/// guarantees that construction happens exactly once (even across threads)
/// using `std::call_once`.
class ImageProxy final : public Image {
public:
    explicit ImageProxy(std::string path) : path_(std::move(path)) {}

    [[nodiscard]] std::string draw() const override {
        ensure_loaded();
        return real_->draw();
    }

    /// True once the expensive subject has been materialised.
    [[nodiscard]] bool is_loaded() const { return real_ != nullptr; }

private:
    void ensure_loaded() const {
        std::call_once(once_, [this] {
            real_ = std::make_unique<RealImage>(path_);
        });
    }

    std::string path_;
    mutable std::once_flag once_;
    mutable std::unique_ptr<RealImage> real_;
};

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_PROXY_HPP
