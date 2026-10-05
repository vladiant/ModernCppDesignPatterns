#ifndef MCPP_COMMAND_COMMANDS_HPP
#define MCPP_COMMAND_COMMANDS_HPP

/// \file commands.hpp
/// \brief Command pattern over an undoable integer accumulator (C++23).
///
/// Showcased idioms:
///  - **`std::expected<void, CommandError>`** on every `execute()`/`undo()`.
///  - **`static operator()`** for a stateless command (`Negate`).
///  - **`if consteval`** inside `safe_div`: a divide-by-zero is a *hard compile
///    error* when operands are known at compile time, but a *runtime
///    `std::expected` error* otherwise.

#include <concepts>
#include <expected>
#include <limits>

namespace mcpp::command {

/// Recoverable command failures.
enum class CommandError { divide_by_zero, nothing_to_undo, overflow };

/// The mutable receiver.
struct Accumulator {
    long long value{0};
};

/// Constexpr division helper showcasing `if consteval`.
/// At runtime, `b == 0` yields `std::unexpected(divide_by_zero)`. During
/// constant evaluation, `b == 0` is made ill-formed (throws in a constexpr
/// context → not a constant expression → compile error).
[[nodiscard]] constexpr std::expected<long long, CommandError> safe_div(long long a, long long b) {
    if (b == 0) {
        if consteval {
            throw "divide by zero in constant evaluation";
        } else {
            return std::unexpected(CommandError::divide_by_zero);
        }
    }
    return a / b;
}

/// Command contract: `execute`/`undo` return `std::expected<void, CommandError>`.
template <class C>
concept Command = requires(C c, Accumulator& acc) {
    { c.execute(acc) } -> std::same_as<std::expected<void, CommandError>>;
    { c.undo(acc) } -> std::same_as<std::expected<void, CommandError>>;
};

/// Stateful command: add a fixed operand. Guards against overflow.
struct AddCommand {
    long long operand;

    [[nodiscard]] std::expected<void, CommandError> execute(Accumulator& acc) const {
        if (would_overflow(acc.value, operand)) {
            return std::unexpected(CommandError::overflow);
        }
        acc.value += operand;
        return {};
    }

    [[nodiscard]] std::expected<void, CommandError> undo(Accumulator& acc) const {
        acc.value -= operand;
        return {};
    }

private:
    static bool would_overflow(long long current, long long delta) {
        if (delta > 0 && current > std::numeric_limits<long long>::max() - delta) {
            return true;
        }
        if (delta < 0 && current < std::numeric_limits<long long>::min() - delta) {
            return true;
        }
        return false;
    }
};

/// Stateful command: divide the accumulator, remembering the previous value for
/// undo. Uses `safe_div` (and thus `if consteval`) for the division.
struct DivideCommand {
    long long divisor;
    long long previous{};

    [[nodiscard]] std::expected<void, CommandError> execute(Accumulator& acc) {
        auto result = safe_div(acc.value, divisor);
        if (!result) {
            return std::unexpected(result.error());
        }
        previous = acc.value;
        acc.value = *result;
        return {};
    }

    [[nodiscard]] std::expected<void, CommandError> undo(Accumulator& acc) const {
        acc.value = previous;
        return {};
    }
};

/// Stateless command via `static operator()`: negate the accumulator.
struct Negate {
    static std::expected<void, CommandError> operator()(Accumulator& acc) {
        acc.value = -acc.value;
        return {};
    }
};

} // namespace mcpp::command

#endif // MCPP_COMMAND_COMMANDS_HPP
