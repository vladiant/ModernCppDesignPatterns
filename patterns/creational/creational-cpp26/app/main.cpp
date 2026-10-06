/// \file main.cpp
/// \brief Narrative demo exercising all five creational patterns (C1–C5) from
///        the split headers. Prints human-readable output and exits 0.

#include <mcpp/creational26/abstract_factory.hpp>
#include <mcpp/creational26/builder.hpp>
#include <mcpp/creational26/factory_method.hpp>
#include <mcpp/creational26/prototype.hpp>
#include <mcpp/creational26/singleton.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <utility>

namespace {

std::string build_error_name(mcpp::creational26::BuildError e) {
    using mcpp::creational26::BuildError;
    switch (e) {
        case BuildError::missing_url:
            return "missing_url";
        case BuildError::invalid_method:
            return "invalid_method";
        case BuildError::empty_header_name:
            return "empty_header_name";
    }
    return "?";
}

}  // namespace

int main() {
    using namespace mcpp::creational26;

    // --- C1 Singleton : = delete("reason") -------------------------------
    std::cout << "== C1 Singleton (= delete(\"reason\")) ==\n";
    AppConfig& cfg = AppConfig::instance();
    cfg.set_app_name("checkout-service");
    cfg.set_worker_threads(8);
    std::cout << "  app_name       = " << cfg.app_name() << "\n";
    std::cout << "  worker_threads = " << cfg.worker_threads() << "\n";
    std::cout << "  same instance? "
              << (&AppConfig::instance() == &cfg ? "yes" : "no") << "\n";
    // The next line is intentionally left commented: copying AppConfig is a
    // compile error whose message *names the fix* (deleted-with-reason idiom):
    //   AppConfig copy = cfg;  // error: ... "take a const& instead ..."
    std::cout << "  (copying AppConfig is a compile error by design)\n";

    // --- C2 Factory Method : move_only_function + std::expected ----------
    std::cout << "\n== C2 Factory Method (move_only_function + expected) ==\n";
    ShapeFactory factory = make_default_shape_factory();
    for (const char* name : {"circle", "square", "hexagon", "trapezoid"}) {
        auto result = factory.create(name, 2.0);
        if (result) {
            std::cout << "  create(\"" << name << "\") -> " << result->describe()
                      << "\n";
        } else {
            std::cout << "  create(\"" << name
                      << "\") -> unexpected(unknown_kind)\n";
        }
    }

    // --- C3 Abstract Factory : concepts over theme types -----------------
    std::cout << "\n== C3 Abstract Factory (concepts over themes) ==\n";
    std::cout << "  light: " << render_dialog(LightTheme{}) << "\n";
    std::cout << "  dark:  " << render_dialog(DarkTheme{}) << "\n";

    // --- C4 Builder : deducing this + validated build() ------------------
    std::cout << "\n== C4 Builder (deducing this + expected) ==\n";
    auto ok = HttpRequestBuilder{}
                  .with_method("POST")
                  .with_url("https://example.com/orders")
                  .with_header("Content-Type", "application/json")
                  .with_body(R"({"id":42})")
                  .build();  // rvalue chain -> fields move out
    if (ok) {
        std::cout << "  built: " << ok->method << " " << ok->url << " ("
                  << ok->headers.size() << " header, body=" << ok->body << ")\n";
    }
    auto bad = HttpRequestBuilder{}.with_method("POST").build();  // no url
    std::cout << "  no-url build -> "
              << (bad ? std::string{"ok"} : build_error_name(bad.error()))
              << "\n";

    // --- C5 Prototype : gof::polymorphic deep clone (no virtual clone()) --
    std::cout << "\n== C5 Prototype (gof::polymorphic deep clone) ==\n";
    PrototypeRegistry registry;
    registry.register_prototype("unit-circle",
                                gof::polymorphic<Figure>(Circle{"blue", 1.0}));
    registry.register_prototype(
        "a4-rect", gof::polymorphic<Figure>(Rectangle{"white", 21.0, 29.7}));

    auto clone = registry.clone("unit-circle");
    std::cout << "  prototype: " << (*clone)->describe() << "\n";
    (*clone)->set_color("red");  // mutate the clone independently
    std::cout << "  mutated clone: " << (*clone)->describe() << "\n";
    auto again = registry.clone("unit-circle");
    std::cout << "  fresh clone:   " << (*again)->describe()
              << "  (prototype unaffected)\n";
    std::cout << "  clone(\"missing\") present? "
              << (registry.clone("missing") ? "yes" : "no") << "\n";

    std::cout << "\nAll five creational patterns demonstrated.\n";
    return 0;
}
