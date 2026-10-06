/// \file decorator_test.cpp
/// \brief Catch2 tests for Decorator (C9): layers owned as gof::polymorphic;
///        copying a decorated stack deep-clones the whole chain.

#include <mcpp/structural26/decorator.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace mcpp::structural26;

TEST_CASE("Base notifier logs the message", "[decorator]") {
    BaseNotifier base;
    auto out = base.send("hi");
    REQUIRE(out == std::vector<std::string>{"log: hi"});
}

TEST_CASE("A single decorator augments the base channel", "[decorator]") {
    SmsDecorator sms{BaseNotifier{}, "555-0100"};
    auto out = sms.send("alert");
    REQUIRE(out == std::vector<std::string>{"log: alert",
                                            "sms to 555-0100: alert"});
}

TEST_CASE("Stacked decorators dispatch in wrap order", "[decorator]") {
    EmailDecorator stack{SmsDecorator{BaseNotifier{}, "555-0100"},
                         "ops@example.com"};
    auto out = stack.send("ping");
    REQUIRE(out ==
            std::vector<std::string>{"log: ping", "sms to 555-0100: ping",
                                     "email to ops@example.com: ping"});
}

TEST_CASE("Copying a polymorphic stack deep-clones the whole chain",
          "[decorator]") {
    gof::polymorphic<Notifier> original{SlackDecorator{
        SmsDecorator{BaseNotifier{}, "555-0100"}, "ops"}};
    gof::polymorphic<Notifier> copy = original;
    // Both independent clones must produce identical transcripts.
    auto a = original->send("x");
    auto b = copy->send("x");
    REQUIRE(a == b);
    REQUIRE(a == std::vector<std::string>{"log: x", "sms to 555-0100: x",
                                          "slack #ops: x"});
}

TEST_CASE("Three stacked decorators compose", "[decorator][edge]") {
    SlackDecorator full{
        EmailDecorator{SmsDecorator{BaseNotifier{}, "n"}, "e"}, "c"};
    auto out = full.send("m");
    REQUIRE(out.size() == 4);
    REQUIRE(out.front() == "log: m");
    REQUIRE(out.back() == "slack #c: m");
}
