/// \file observer_test.cpp
/// \brief Catch2 tests for the Observer pattern.

#include <mcpp/observer/observer.hpp>
#include <mcpp/observer/subject.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace mcpp::observer;

namespace {
Observer recorder(int priority, std::string name, std::vector<std::string>& log) {
    return Observer{priority, name, [name, &log](const Event& e) {
                        log.push_back(name + ":" + e.payload);
                    }};
}
} // namespace

TEST_CASE("Dispatch happens in <=> order regardless of insertion order",
          "[observer][order]") {
    std::vector<std::string> log;
    Subject subject;

    subject.subscribe(recorder(30, "c", log));
    subject.subscribe(recorder(10, "a", log));
    subject.subscribe(recorder(20, "b", log));

    subject.publish(Event{"t", "x"});

    REQUIRE(log == std::vector<std::string>{"a:x", "b:x", "c:x"});
}

TEST_CASE("Tie-break by name for equal priority", "[observer][order]") {
    std::vector<std::string> log;
    Subject subject;

    subject.subscribe(recorder(10, "zeta", log));
    subject.subscribe(recorder(10, "alpha", log));

    subject.publish(Event{"t", "x"});

    REQUIRE(log == std::vector<std::string>{"alpha:x", "zeta:x"});
}

TEST_CASE("Duplicate subscribe is rejected (dedup)", "[observer][dedup]") {
    std::vector<std::string> log;
    Subject subject;

    REQUIRE(subject.subscribe(recorder(10, "a", log)));
    REQUIRE_FALSE(subject.subscribe(recorder(10, "a", log))); // same (priority,name)
    REQUIRE(subject.size() == 1);

    subject.publish(Event{"t", "x"});
    REQUIRE(log.size() == 1); // fired once, not twice
}

TEST_CASE("Unsubscribed observer no longer receives events", "[observer][unsubscribe]") {
    std::vector<std::string> log;
    Subject subject;

    subject.subscribe(recorder(10, "a", log));
    subject.subscribe(recorder(20, "b", log));

    REQUIRE(subject.unsubscribe(Observer{10, "a", nullptr}));
    REQUIRE_FALSE(subject.unsubscribe(Observer{99, "missing", nullptr}));

    subject.publish(Event{"t", "x"});
    REQUIRE(log == std::vector<std::string>{"b:x"});
}

TEST_CASE("operator<=> and == compare by (priority, name) only", "[observer][spaceship]") {
    Observer x{10, "a", nullptr};
    Observer y{10, "a", [](const Event&) {}}; // different callback, same identity
    Observer z{11, "a", nullptr};

    REQUIRE(x == y);
    REQUIRE((x <=> y) == std::strong_ordering::equal);
    REQUIRE((x <=> z) == std::strong_ordering::less);
}
