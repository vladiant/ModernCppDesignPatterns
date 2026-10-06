/// \file main.cpp
/// \brief Narrative demo exercising all nine behavioral patterns (C13–C21)
///        from the split headers. Prints human-readable output and exits 0.

#include <mcpp/behavioral26/chain.hpp>
#include <mcpp/behavioral26/command.hpp>
#include <mcpp/behavioral26/interpreter.hpp>
#include <mcpp/behavioral26/iterator.hpp>
#include <mcpp/behavioral26/memento.hpp>
#include <mcpp/behavioral26/observer.hpp>
#include <mcpp/behavioral26/state.hpp>
#include <mcpp/behavioral26/strategy.hpp>
#include <mcpp/behavioral26/template_method.hpp>

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <version>  // exposes __cpp_lib_generator before we probe it

#if defined(__cpp_lib_generator)
#include <generator>
#endif

int main() {
    using namespace mcpp::behavioral26;

    std::cout << "== Strategy ==\n";
    std::vector<int> v{3, 1, 4, 1, 5, 9, 2, 6};
    sort_with(v, [](int a, int b) { return a < b; });
    std::cout << "  ascending:  " << join(v) << "\n";
    sort_with(v, [](int a, int b) { return a > b; });
    std::cout << "  descending: " << join(v) << "\n";
    Checkout checkout{[](double s) { return s; }};
    std::cout << "  regular:    " << checkout.total(100) << "\n";
    checkout.set_pricing([rate = 0.8](double s) { return s * rate; });
    std::cout << "  sale (-20%): " << checkout.total(100) << "\n";

    std::cout << "\n== Observer ==\n";
    Signal<std::string_view, int> price_changed;
    auto logger = price_changed.connect([](std::string_view sym, int p) {
        std::cout << "  logger: " << sym << " -> " << p << "\n";
    });
    price_changed.connect(
        [alerts = std::make_unique<int>(0)](std::string_view sym, int p) mutable {
            if (p > 100)
                std::cout << "  alert #" << ++*alerts << ": " << sym
                          << " above 100\n";
        });
    price_changed.emit("ACME", 90);
    price_changed.emit("ACME", 120);
    price_changed.disconnect(logger);
    price_changed.emit("ACME", 130);

    std::cout << "\n== Command ==\n";
    std::string doc;
    History history;
    auto append = [&doc](std::string s) {
        return Command{"append " + s, [&doc, s] { doc += s; },
                       [&doc, n = s.size()] { doc.resize(doc.size() - n); }};
    };
    history.execute(append("Hello"));
    history.execute(append(", world"));
    std::cout << "  after 2 commands: '" << doc << "'\n";
    history.undo();
    std::cout << "  after undo:       '" << doc << "'\n";
    history.redo();
    std::cout << "  after redo:       '" << doc << "'\n";

    std::cout << "\n== State ==\n";
    PlayerState state = Idle{};
    for (PlayerEvent ev : {PlayerEvent{Play{}}, PlayerEvent{Pause{}},
                           PlayerEvent{Play{}}, PlayerEvent{Stop{}},
                           PlayerEvent{Pause{}}}) {
        state = transition(state, ev);
        std::cout << "  -> " << describe(state) << "\n";
    }

    std::cout << "\n== Visitor / Interpreter ==\n";
    Expr e = bin('*', bin('+', num(1), num(2)), bin('-', num(10), num(4)));
    std::cout << "  " << show(e) << " = " << eval(e) << "\n";

    std::cout << "\n== Template Method ==\n";
    CsvExporter{}.run();
    JsonExporter{}.run();

    std::cout << "\n== Iterator ==\n";
    auto tree = node(4, node(2, leaf(1), leaf(3)), node(6, leaf(5), leaf(7)));
    std::cout << "  in-order:";
#if defined(__cpp_lib_generator)
    for (int x : inorder(tree.get())) std::cout << ' ' << x;
#else
    inorder(tree.get(), [](int x) { std::cout << ' ' << x; });
#endif
    std::cout << "\n";

    std::cout << "\n== Chain of Responsibility ==\n";
    ApprovalChain chain;
    chain
        .then([](const Request& r) -> std::optional<std::string> {
            if (r.amount <= 100)
                return "team lead approved " + std::to_string(r.amount);
            return std::nullopt;
        })
        .then([](const Request& r) -> std::optional<std::string> {
            if (r.amount <= 1000)
                return "manager approved " + std::to_string(r.amount);
            return std::nullopt;
        });
    for (int amount : {50, 500, 5000})
        std::cout << "  " << chain.handle({"alice", amount}) << "\n";

    std::cout << "\n== Memento ==\n";
    Editor editor;
    editor.type("Hello");
    auto checkpoint = editor.save();
    editor.type(", world");
    std::cout << "  before restore: '" << editor.text() << "'\n";
    editor.restore(checkpoint);
    std::cout << "  after restore:  '" << editor.text() << "'\n";

    return 0;
}
