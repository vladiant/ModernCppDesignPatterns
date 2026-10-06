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
}  // namespace

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
