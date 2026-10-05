/// \file main.cpp
/// \brief Observer demo: <=>-ordered dispatch, dedup, and unsubscribe.

#include <mcpp/observer/observer.hpp>
#include <mcpp/observer/subject.hpp>

#include <format>
#include <iostream>

int main() {
    using namespace mcpp::observer;

    Subject subject;

    auto make_logger = [](const std::string& who) {
        return [who](const Event& e) {
            std::cout << std::format("  {} received [{}] {}\n", who, e.topic, e.payload);
        };
    };

    // Subscribe out of priority order and with distinct names.
    subject.subscribe(Observer{20, "audit", make_logger("audit")});
    subject.subscribe(Observer{10, "metrics", make_logger("metrics")});
    subject.subscribe(Observer{10, "alerts", make_logger("alerts")});

    std::cout << "Publishing to " << subject.size() << " observers (in <=> order):\n";
    subject.publish(Event{"temperature", "42C"});

    // A duplicate (same priority+name) is rejected regardless of callback.
    const bool added = subject.subscribe(Observer{10, "alerts", make_logger("alerts-dup")});
    std::cout << std::format("\nDuplicate subscribe accepted? {} (size still {})\n", added,
                             subject.size());

    // Unsubscribe and re-publish.
    subject.unsubscribe(Observer{10, "alerts", nullptr});
    std::cout << "\nAfter unsubscribing 'alerts':\n";
    subject.publish(Event{"temperature", "7C"});

    return 0;
}
