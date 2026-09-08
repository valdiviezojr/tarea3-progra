
#ifndef CORENUMERIC_CORE_NUMERIC_H
#define CORENUMERIC_CORE_NUMERIC_H

#include <concepts>
#include <iterator>
#include <cstddef>
#include <stdexcept>

namespace core_numeric {

    template <typename C>
    concept Iterable= requires(C c) {
        std::begin(c);
        std::end(c);
    };

    template <typename T>
    concept Addable= requires(T a, T b) {
        { a+b } -> std::same_as<T>;
    };

    template <typename T>
    concept Divisible= requires(T a, std::size_t n) {
        { a/n } -> std::same_as<T>;
    };

    //algoritmos basicos
    template <Iterable C>
    requires Addable<typename C::value_type>
    auto sum(const C& container) {
        using T= typename C::value_type;
        T result{};

        for (const auto& value: container) {
            result= result+ value;
        }
        return result;
    }


    template <Iterable C>
    requires Addable<typename C::value_type> && Divisible<typename C::value_type>
    auto mean(const C& container) {
        using T=typename C::value_type;

        std::size_t count= 0;
        for (auto it= std::begin(container); it!= std::end(container); ++it) {
            ++count;
        }

        if (count==0) {
            throw std::invalid_argument("El contenedor no puede estar vacío para calcular la media");
        }

        //reutiliza sum
        T total= sum(container);

        return total/count;
    }

}


#endif //CORENUMERIC_CORE_NUMERIC_H