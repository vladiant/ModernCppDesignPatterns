/// \file singleton.hpp
/// \brief Singleton (C1) — `= delete("reason")` copy/move with a *readable*
///        deleted-function diagnostic, plus a Meyers-style single accessor.

#ifndef MCPP_CREATIONAL26_SINGLETON_HPP
#define MCPP_CREATIONAL26_SINGLETON_HPP

#include <cstdint>
#include <string>

/// \def MCPP_DELETE_MSG
/// \brief Emit a deleted function *with a reason string* (C++26, P2573) when the
///        toolchain supports it, else fall back to a plain `= delete`.
///
/// The reason string is the whole point of C1: a copy attempt should yield a
/// diagnostic that explains *why* copying is forbidden. The reason text is
/// always present in the source (so a reviewer sees the idiom); the compiler
/// surfaces it verbatim only where `__cpp_deleted_function` (>= 202403L) is
/// defined. g++-14 `-std=c++26` does **not** define it yet (GCC 15+), so the
/// fallback keeps the project building while preserving the intent.
#if defined(__cpp_deleted_function) && __cpp_deleted_function >= 202403L
#define MCPP_DELETE_MSG(msg) delete (msg)
#else
#define MCPP_DELETE_MSG(msg) delete
#endif

namespace mcpp::creational26 {

/// Process-wide application configuration — a Meyers singleton.
///
/// The copy/move operations are deleted *with a reason*: any attempt to copy an
/// `AppConfig` produces a diagnostic that names the fix ("take a `const&`
/// instead"), instead of a cryptic "use of deleted function" with no guidance.
class AppConfig {
public:
    /// The single, lazily-constructed, process-wide instance.
    ///
    /// Thread-safe first-call initialization is guaranteed by the C++ standard
    /// for function-local statics.
    static AppConfig& instance() {
        static AppConfig cfg;
        return cfg;
    }

    // --- The C++26 idiom: deleted with a human-readable reason -------------
    AppConfig(const AppConfig&) =
        MCPP_DELETE_MSG("AppConfig is a process-wide singleton; take a const& "
                        "instead of copying it");
    AppConfig& operator=(const AppConfig&) =
        MCPP_DELETE_MSG("AppConfig is a process-wide singleton; assign through "
                        "its setters, do not copy the instance");
    AppConfig(AppConfig&&) =
        MCPP_DELETE_MSG("AppConfig is a process-wide singleton and cannot be "
                        "moved out of its storage");
    AppConfig& operator=(AppConfig&&) =
        MCPP_DELETE_MSG("AppConfig is a process-wide singleton and cannot be "
                        "move-assigned");

    /// Current application name (mutable shared state).
    [[nodiscard]] const std::string& app_name() const noexcept {
        return app_name_;
    }
    void set_app_name(std::string name) { app_name_ = std::move(name); }

    /// Current worker-thread count.
    [[nodiscard]] std::uint32_t worker_threads() const noexcept {
        return worker_threads_;
    }
    void set_worker_threads(std::uint32_t n) noexcept { worker_threads_ = n; }

private:
    AppConfig() = default;
    ~AppConfig() = default;

    std::string app_name_{"mcpp-app"};
    std::uint32_t worker_threads_{4};
};

}  // namespace mcpp::creational26

#endif  // MCPP_CREATIONAL26_SINGLETON_HPP
