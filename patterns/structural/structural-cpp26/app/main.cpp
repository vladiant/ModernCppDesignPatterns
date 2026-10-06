/// \file main.cpp
/// \brief Narrative demo exercising all seven structural patterns (C6–C12)
///        from the split headers. Prints human-readable output and exits 0.

#include <mcpp/structural26/adapter.hpp>
#include <mcpp/structural26/bridge.hpp>
#include <mcpp/structural26/composite.hpp>
#include <mcpp/structural26/decorator.hpp>
#include <mcpp/structural26/facade.hpp>
#include <mcpp/structural26/flyweight.hpp>
#include <mcpp/structural26/proxy.hpp>

#include <iostream>
#include <string>

int main() {
    using namespace mcpp::structural26;

    std::cout << "== Adapter (concept as target interface) ==\n";
    CelsiusThermometer modern{"indoor", 21.5};
    FahrenheitAdapter adapted{LegacyFahrenheitSensor{"outdoor", 68.0}};
    std::cout << "  " << report(modern) << "\n";
    std::cout << "  " << report(adapted) << "\n";

    std::cout << "\n== Bridge (gof::polymorphic<Impl> member) ==\n";
    Window window{"Circle", VectorRenderer{}};
    std::cout << "  " << window.draw() << "\n";
    Window copy = window;  // deep-copies the implementor
    copy.set_renderer(RasterRenderer{});
    std::cout << "  original: " << window.draw() << "\n";
    std::cout << "  copy:     " << copy.draw() << "\n";

    std::cout << "\n== Composite (variant tree + deducing-this lambda) ==\n";
    Node tree = directory(
        "root",
        {file("readme.md", 120),
         directory("src", {file("main.cpp", 900), file("util.hpp", 300)}),
         file("LICENSE", 1100)});
    std::cout << render(tree);
    std::cout << "  total size:  " << total_size(tree) << " bytes\n";
    std::cout << "  file count:  " << count_files(tree) << "\n";

    std::cout << "\n== Decorator (layers owned as gof::polymorphic) ==\n";
    gof::polymorphic<Notifier> stack{EmailDecorator{
        SmsDecorator{BaseNotifier{}, "555-0100"}, "ops@example.com"}};
    gof::polymorphic<Notifier> deep_copy = stack;  // clones the whole chain
    for (const std::string& line : stack->send("disk almost full"))
        std::cout << "  " << line << "\n";
    std::cout << "  (deep copy dispatches independently):\n";
    for (const std::string& line : deep_copy->send("ping"))
        std::cout << "  " << line << "\n";

    std::cout << "\n== Facade (std::expected monadic chain) ==\n";
    OrderFacade facade;
    for (const Order& order :
         {Order{"book", 2, 100.0}, Order{"book", 0, 100.0},
          Order{"book", 3, 5.0}}) {
        auto receipt = facade.place_receipt(order);
        if (receipt)
            std::cout << "  ok:   " << *receipt << "\n";
        else
            std::cout << "  fail: " << to_string(receipt.error()) << "\n";
    }

    std::cout << "\n== Flyweight (shared_ptr<const T> cache) ==\n";
    GlyphCache glyphs;
    const std::string text = "mississippi";
    const int width = layout_width(glyphs, text);
    std::cout << "  laid out \"" << text << "\" width=" << width
              << " using only " << glyphs.size() << " distinct glyphs\n";

    std::cout << "\n== Proxy (std::call_once lazy load) ==\n";
    RealImage::reset_load_count();
    ImageProxy image{"photo.png"};
    std::cout << "  loaded before use? " << std::boolalpha << image.is_loaded()
              << "\n";
    std::cout << "  draw -> " << image.draw() << "\n";
    std::cout << "  draw -> " << image.draw() << "\n";
    std::cout << "  real loads performed: " << RealImage::load_count() << "\n";

    return 0;
}
