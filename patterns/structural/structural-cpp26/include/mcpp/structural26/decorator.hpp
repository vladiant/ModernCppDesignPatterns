/// \file decorator.hpp
/// \brief Decorator (C9) — a `Notifier` base wrapped by SMS/Email/Slack
///        decorators. Each layer owns its wrapped component as a
///        `gof::polymorphic<Notifier>`, so copying a decorated stack
///        deep-clones the whole chain with **no manual clone plumbing**.

#ifndef MCPP_STRUCTURAL26_DECORATOR_HPP
#define MCPP_STRUCTURAL26_DECORATOR_HPP

#include "compat.hpp"

#include <string>
#include <utility>
#include <vector>

namespace mcpp::structural26 {

/// Component interface: send a message, return the human-readable transcript
/// line(s) describing what was dispatched.
class Notifier {
public:
    virtual ~Notifier() = default;
    [[nodiscard]] virtual std::vector<std::string> send(
        const std::string& message) const = 0;
};

/// Concrete base component: the core notifier every decorator wraps around.
class BaseNotifier final : public Notifier {
public:
    [[nodiscard]] std::vector<std::string> send(
        const std::string& message) const override {
        return {"log: " + message};
    }
};

/// Base class for decorators: owns the wrapped component **by value** via
/// `gof::polymorphic`, forwarding and augmenting its behaviour.
class NotifierDecorator : public Notifier {
public:
    template <class N>
    explicit NotifierDecorator(N inner)
        : inner_(gof::polymorphic<Notifier>(std::move(inner))) {}

protected:
    [[nodiscard]] std::vector<std::string> forward(
        const std::string& message) const {
        return inner_->send(message);
    }

private:
    gof::polymorphic<Notifier> inner_;
};

/// Adds an SMS channel on top of whatever it wraps.
class SmsDecorator final : public NotifierDecorator {
public:
    template <class N>
    SmsDecorator(N inner, std::string number)
        : NotifierDecorator(std::move(inner)), number_(std::move(number)) {}

    [[nodiscard]] std::vector<std::string> send(
        const std::string& message) const override {
        auto out = forward(message);
        out.push_back("sms to " + number_ + ": " + message);
        return out;
    }

private:
    std::string number_;
};

/// Adds an Email channel on top of whatever it wraps.
class EmailDecorator final : public NotifierDecorator {
public:
    template <class N>
    EmailDecorator(N inner, std::string address)
        : NotifierDecorator(std::move(inner)), address_(std::move(address)) {}

    [[nodiscard]] std::vector<std::string> send(
        const std::string& message) const override {
        auto out = forward(message);
        out.push_back("email to " + address_ + ": " + message);
        return out;
    }

private:
    std::string address_;
};

/// Adds a Slack channel on top of whatever it wraps.
class SlackDecorator final : public NotifierDecorator {
public:
    template <class N>
    SlackDecorator(N inner, std::string channel)
        : NotifierDecorator(std::move(inner)), channel_(std::move(channel)) {}

    [[nodiscard]] std::vector<std::string> send(
        const std::string& message) const override {
        auto out = forward(message);
        out.push_back("slack #" + channel_ + ": " + message);
        return out;
    }

private:
    std::string channel_;
};

}  // namespace mcpp::structural26

#endif  // MCPP_STRUCTURAL26_DECORATOR_HPP
