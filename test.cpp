#include "core_numeric.h"
#include <iostream>
#include <vector>
#include <list>
#include <string>

struct SoloSumable {
    int val= 0;
    SoloSumable operator+(const SoloSumable& other) const {
        return SoloSumable{val+ other.val};
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

    return 0;
}