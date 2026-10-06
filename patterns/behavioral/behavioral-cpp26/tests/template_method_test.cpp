/// \file template_method_test.cpp
/// \brief Catch2 tests for Template Method (C18): deducing-this skeleton calls
///        derived hooks in order (open -> write_rows -> close).

#include <mcpp/behavioral26/template_method.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace mcpp::behavioral26;

namespace {
/// A probe exporter that records hook invocation order instead of printing.
struct ProbeExporter : Exporter {
    std::vector<std::string>* log;
    void open() { log->push_back("open"); }
    void write_rows() { log->push_back("write_rows"); }
    void close() { log->push_back("close"); }
};

/// An incomplete exporter missing two mandatory hooks. The `requires` clause
/// on Exporter::run must reject it (negative side of the C18 hook contract).
struct PartialExporter : Exporter {
    void open() {}  // no write_rows(), no close()
};

/// Named concept wrapping the run() callability probe. (A named concept gives
/// g++-14 the SFINAE context it needs; a bare requires-expression inside a
/// static_assert is a hard error on this toolchain.)
template <class T>
concept Runnable = requires(T t) { t.run(); };
}  // namespace

// C18 idiom (negative branch): the requires-constrained run() is callable only
// when all hooks exist, and is SFINAE-rejected otherwise — no hard error.
static_assert(Runnable<CsvExporter>);
static_assert(Runnable<JsonExporter>);
static_assert(!Runnable<PartialExporter>);
static_assert(!Runnable<Exporter>);

TEST_CASE("run invokes hooks in open/write_rows/close order",
          "[template_method]") {
    std::vector<std::string> log;
    ProbeExporter{{}, &log}.run();
    REQUIRE(log == std::vector<std::string>{"open", "write_rows", "close"});
}

TEST_CASE("concrete exporters satisfy the hook requirement",
          "[template_method]") {
    // If these compile and run, the requires-clause hook contract is met.
    CsvExporter{}.run();
    JsonExporter{}.run();
    SUCCEED("CsvExporter and JsonExporter both run");
}
