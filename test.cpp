#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <concepts>
#include <iterator>
#include <cstddef>
#include <stdexcept>
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

    //Concept adicional que restringe tipos numéricos en variadic
    template <typename T>
    concept NumericType = Addable<T> && Divisible<T>;

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

    //---------------------------------------------------------------------------------------
    // VARIADIC TEMPLATES, FOLD EXPRESSIONS e IF CONSTEXPR

    template <Addable First, Addable... Args>
    requires (std::same_as<First, Args> && ...)
    auto sum_variadic(First first, Args... args) {
        return (first + ... + args);
    }

    template <NumericType First, NumericType... Args>
    requires (std::same_as<First, Args> && ...)
    auto mean_variadic(First first, Args... args) {
        constexpr std::size_t N = sizeof...(Args) + 1;
        auto total = sum_variadic(first, args...);
        return total / N;
    }

    template <NumericType First, NumericType... Args>
    requires VarianceCompatible<First> && (std::same_as<First, Args> && ...)
    auto variance_variadic(First first, Args... args) {
        constexpr std::size_t N = sizeof...(Args) + 1;
        auto mu = mean_variadic(first, args...);

        // Suma usando fold expression
        auto suma_cuadrados = (( (args - mu) * (args - mu) ) + ... + ((first - mu) * (first - mu)));

        if constexpr (std::is_integral_v<First>) {
            //Enteros -> división entera
            return suma_cuadrados / N;
        } else {
            //Con decimales
            return suma_cuadrados / static_cast<double>(N);
        }
    }

    template <Comparable First, Comparable... Args>
    requires (std::same_as<First, Args> && ...)
    auto max_variadic(First first, Args... args) {
        auto select_max = [](const auto& a, const auto& b) {
            return (a > b) ? a : b;
        };
        First result = first;
        ((result = select_max(result, args)), ...);
        return result;
    }

}




struct SoloSumable {
    int val= 0;
    SoloSumable operator+(const SoloSumable& other) const {
        return SoloSumable{val+ other.val};
    }
};

class Vector3D {
private:
    double x;
    double y;
    double z;

public:
    Vector3D(double x = 0, double y = 0, double z = 0)
        : x(x), y(y), z(z) {}

    double magnitudCuadrada() const {
        return x * x + y * y + z * z;
    }

    bool operator>(const Vector3D& other) const {
        return magnitudCuadrada() > other.magnitudCuadrada();
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3D& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

int main() {
    std::cout<< "=== PRUEBAS VÁLIDAS ===\n";

    //sum con vector<int>
    std::vector<int> v_int= {1, 2, 3, 4, 5};
    std::cout<< "sum(v_int): "<< core_numeric::sum(v_int)<< "\n"; //15

    //sum con list<double>
    std::list<double> l_double= {10.5, 20.5, 30.0};
    std::cout <<"sum(l_double): "<< core_numeric::sum(l_double) << "\n"; //61.0

    //mean con vector<double>
    std::vector<double> v_double= {2.0, 4.0, 6.0, 8.0};
    std::cout<< "mean(v_double): " << core_numeric::mean(v_double)<< "\n"; //5.0

    //mean con vector<float>
    std::vector<float> v_float= {10.0f, 20.0f, 30.0f};
    std::cout<< "mean(v_float): " << core_numeric::mean(v_float) << "\n"; //20.0
    std::cout<< "\nTodas las pruebas válidas pasaron correctamente.\n";

    // =======================
    // PRUEBAS PERSONA 2
    // =======================

    // variance
    std::vector<double> datos_varianza = {1.0, 2.0, 3.0};
    std::cout << "variance(datos_varianza): "
              << core_numeric::variance(datos_varianza)
              << "\n";

    // max con enteros
    std::vector<int> datos_max = {4, 9, 2, 15, 7};
    std::cout << "max(datos_max): "
              << core_numeric::max(datos_max)
              << "\n";

    // max con objetos Vector3D
    std::vector<Vector3D> vectores = {
        Vector3D(1, 0, 0),
        Vector3D(3, 4, 0),
        Vector3D(2, 2, 2)
    };

    std::cout << "max(vectores): "
              << core_numeric::max(vectores)
              << "\n";

    // transform_reduce
    std::vector<double> datos_transform = {1.0, 2.0, 3.0};

    auto resultado = core_numeric::transform_reduce(
        datos_transform,
        [](double x) {
            return x * x;
        }
    );

    std::cout << "transform_reduce(datos_transform): "
              << resultado
              << "\n";


    // ===============
    // CASOS INVÁLIDOS
    // ===============

    /*
    // CASO 1: sum con un tipo que NO es Iterable
    int no_iterable = 42;
    core_numeric::sum(no_iterable);
    // FALLA: Concept 'Iterable'
    // Explicación: int no cuenta con std::begin() ni std::end()
    */

    /*
    // CASO 2: sum con elementos no Addable (ej. std::string con retorno T)
    // std::string + std::string devuelve std::string, pero si un tipo no tiene operator+
    struct NoSuma {};
    std::vector<NoSuma> v_nosuma(3);
    core_numeric::sum(v_nosuma);
    // FALLA: Concept 'Addable<T>'
    // Explicación: El tipo 'NoSuma' no define el operador operator+(NoSuma, NoSuma)
    */

    /*
    // CASO 3: mean con elementos no Divisible
    std::vector<SoloSumable> v_solosuma = {SoloSumable{2}, SoloSumable{4}};
    core_numeric::mean(v_solosuma);
    // FALLA: Concept 'Divisible<T>'
    // Explicación: 'SoloSumable' cumple con 'Addable', pero no sobrecarga el
    // operador / contra std::size_t devolviendo el mismo tipo
    */

    /*
    // CASO 4: mean con contenedor no Iterable
    double escalar = 3.14;
    core_numeric::mean(escalar);
    // FALLA: Concept 'Iterable'
    // Explicación: double no posee funciones begin/end de iteración
    */

    /*
    // CASO 5: variance con elementos incompatibles
    struct NoVariance {
        int val;
    };

    std::vector<NoVariance> datos_no_variance(3);
    core_numeric::variance(datos_no_variance);

    // FALLA: Concepts 'Addable<T>' y 'VarianceCompatible<T>'
    // Explicación: NoVariance no define los operadores +, -, * y /
    // necesarios para calcular la varianza.
    */


    /*
        // CASO 6: max con std::string
        std::vector<std::string> palabras = {"hola", "mundo", "utec"};
        core_numeric::max(palabras);

        // FALLA: Concept 'Comparable<T>'
        // Explicación: Comparable fue restringido para que std::string
        // no sea aceptado por max, como pide el enunciado.
    */


    /*
        // CASO 7: max con objetos que no se pueden comparar
        struct NoComparable {
            int valor;
        };

        std::vector<NoComparable> objetos = {
            {1}, {2}, {3}
        };

        core_numeric::max(objetos);

        // FALLA: Concept 'Comparable<T>'
        // Explicación: NoComparable no define el operador >.
    */


    /*
        // CASO 8: transform_reduce con un tipo no Iterable
        int valor = 5;

        core_numeric::transform_reduce(
            valor,
            [](int x) {
                return x * x;
            }
        );

        // FALLA: Concept 'Iterable'
        // Explicación: int no posee std::begin() ni std::end().
    */

    //-------------------------------------------------------------------------
    // PRUEBAS PERSONA 3 (variadic templates y if constexpr)

    std::cout << "\n=== PRUEBAS VARIÁDICAS VÁLIDAS ===\n";

    // 1. sum_variadic
    auto s1 = core_numeric::sum_variadic(1, 2, 33, 4);
    std::cout << "sum_variadic(1, 2, 33, 4): " << s1 << "\n"; // 40

    // 2. mean_variadic
    auto s2 = core_numeric::mean_variadic(2.0, 4.0, 6.0, 8.0);
    std::cout << "mean_variadic(2.0, 4.0, 6.0, 8.0): " << s2 << "\n"; // 5.0

    // 3. variance_variadic con if constexpr (flotante vs entero)
    auto s3_float = core_numeric::variance_variadic(1.0, 2.0, 3.0, 4.0);
    std::cout << "variance_variadic (double): " << s3_float << "\n"; // 1.25

    auto s3_int = core_numeric::variance_variadic(1.0, 2.0, 3.0, 4.0);
    std::cout << "variance_variadic (int - if constexpr): " << s3_int << "\n"; // 1

    // 4. max_variadic
    auto s4 = core_numeric::max_variadic(1.0, 2.7, 3.0, 0.4);
    std::cout << "max_variadic(1.0, 2.7, 3.0, 0.4): " << s4 << "\n"; // 3.0


    //-------------------------------------------------------------------------------------
    // PRUEBAS INVÁLIDAS (casos de error de compilacion)

    /*
    // CASO V1: sum_variadic con tipos heterogéneos
    auto err1 = core_numeric::sum_variadic(1, 2.5, 3);
    // FALLA: Cláusula requires (std::same_as<First, Args> && ...)
    // Explicación: Todos los argumentos pasados a la función variádica
    // deben ser estrictamente del mismo tipo.
    */

    /*
    // CASO V2: mean_variadic con tipo no numérico (sin operador /)
    struct NoDivisible {
        int val;
        NoDivisible operator+(const NoDivisible& o) const { return {val + o.val}; }
    };
    auto err2 = core_numeric::mean_variadic(NoDivisible{1}, NoDivisible{2});
    // FALLA: Concept 'NumericType' / 'Divisible'
    // Explicación: El tipo NoDivisible soporta suma pero no soporta
    // división por std::size_t.
    */

    /*
    // CASO V3: max_variadic con std::string
    auto err3 = core_numeric::max_variadic(std::string("a"), std::string("b"));
    // FALLA: Concept 'Comparable'
    // Explicación: El concepto Comparable impide el uso de std::string.
    */

    return 0;
}