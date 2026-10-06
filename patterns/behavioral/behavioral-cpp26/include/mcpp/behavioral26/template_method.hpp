/// \file template_method.hpp
/// \brief Template Method (C18) — the skeleton lives in the base; *deducing
///        this* calls derived hooks without virtuals or CRTP, and a `requires`
///        clause documents the mandatory hooks.

#ifndef MCPP_BEHAVIORAL26_TEMPLATE_METHOD_HPP
#define MCPP_BEHAVIORAL26_TEMPLATE_METHOD_HPP

#include <iostream>

namespace mcpp::behavioral26 {

/// Base skeleton. `run` dispatches to the derived type's hooks through the
/// explicit object parameter; the `requires` clause makes the hook contract
/// part of the signature.
class Exporter {
public:
    void run(this auto&& self)
        requires requires { self.open(); self.write_rows(); self.close(); }
    {
        self.open();
        self.write_rows();
        self.close();
    }
};

struct CsvExporter : Exporter {
    void open() { std::cout << "  csv: open file, write header\n"; }
    void write_rows() { std::cout << "  csv: a,b,c\n"; }
    void close() { std::cout << "  csv: close\n"; }
};

struct JsonExporter : Exporter {
    void open() { std::cout << "  json: [\n"; }
    void write_rows() { std::cout << "  json:   {\"a\":1}\n"; }
    void close() { std::cout << "  json: ]\n"; }
};

}  // namespace mcpp::behavioral26

#endif  // MCPP_BEHAVIORAL26_TEMPLATE_METHOD_HPP
