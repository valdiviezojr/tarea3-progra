#ifndef CORENUMERIC_CORE_NUMERIC_H
#define CORENUMERIC_CORE_NUMERIC_H

#include <concepts>
#include <iterator>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace core_numeric {

    template <typename C>
    concept Iterable = requires(C c) {
        std::begin(c);
        std::end(c);
    };

    template <typename T>
    concept Addable = requires(T a, T b) {
        { a + b } -> std::same_as<T>;
    };

    template <typename T>
    concept Divisible = requires(T a, std::size_t n) {
        { a / n } -> std::same_as<T>;
    };

    template <typename T>
    concept VarianceCompatible = requires(T a, T b, std::size_t n) {
        { a - b } -> std::same_as<T>;
        { a * b } -> std::same_as<T>;
        { a / n } -> std::same_as<T>;
    };

    template <typename T>
    concept Comparable =
    (!std::same_as<std::remove_cvref_t<T>, std::string>) &&
    requires(T a, T b) {
        { a > b } -> std::convertible_to<bool>;
    };

    // ALGORITMOS BÁSICOS

    template <Iterable C>
    requires Addable<typename C::value_type>
    auto sum(const C& container) {
        using T = typename C::value_type;
        T result{};

        for (const auto& value : container) {
            result = result + value;
        }

        return result;
    }


    template <Iterable C>
    requires Addable<typename C::value_type> &&
             Divisible<typename C::value_type>
    auto mean(const C& container) {
        using T = typename C::value_type;

        std::size_t count = 0;

        for (auto it = std::begin(container);
             it != std::end(container);
             ++it) {
            ++count;
        }

        if (count == 0) {
            throw std::invalid_argument(
                "El contenedor no puede estar vacío para calcular la media"
            );
        }

        // reutiliza sum
        T total = sum(container);

        return total / count;
    }


    template <Iterable C>
    requires Addable<typename C::value_type> &&
             VarianceCompatible<typename C::value_type>
    auto variance(const C& container) {
        using T = typename C::value_type;

        std::size_t count = 0;

        for (auto it = std::begin(container);
             it != std::end(container);
             ++it) {
            ++count;
        }

        if (count == 0) {
            throw std::invalid_argument(
                "El contenedor no puede estar vacío para calcular la varianza"
            );
        }

        // reutiliza mean
        T promedio = mean(container);

        T suma{};

        for (const auto& value : container) {
            T diferencia = value - promedio;
            suma = suma + diferencia * diferencia;
        }

        return suma / count;
    }


    template <Iterable C>
    requires Comparable<typename C::value_type>
    auto max(const C& container) {
        using T = typename C::value_type;

        auto it = std::begin(container);

        if (it == std::end(container)) {
            throw std::invalid_argument(
                "El contenedor no puede estar vacío para calcular el máximo"
            );
        }

        T maximo = *it;
        ++it;

        for (; it != std::end(container); ++it) {
            if (*it > maximo) {
                maximo = *it;
            }
        }
        return maximo;
    }

    template <Iterable C, typename Func>
    requires Addable<typename C::value_type>
    auto transform_reduce(const C& container, Func func) {
        using T = typename C::value_type;

        if (std::begin(container) == std::end(container)) {
            throw std::invalid_argument(
                "El contenedor no puede estar vacío para transform_reduce"
            );
        }

        T resultado{};

        for (const auto& value : container) {
            resultado = resultado + func(value);
        }

        return resultado;
    }

}

#endif // CORENUMERIC_CORE_NUMERIC_H