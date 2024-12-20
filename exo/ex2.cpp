#include <iostream>
#include <fstream>
#include "CLHEP/Random/MTwistEngine.h"

int main() {
    CLHEP::MTwistEngine generator;

    generator.saveStatus("save/status_initial.txt");
    std::cout << "Statut initial sauvegardé dans 'status_initial.txt'.\n";

    std::cout << "Première séquence de nombres aléatoires :\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << generator.flat() << std::endl;
    }

    generator.restoreStatus("save/status_initial.txt");
    std::cout << "\nStatut initial restauré.\n";

    std::cout << "Reproduction de la première séquence :\n";
    for (int i = 0; i < 10; ++i) {
        std::cout << generator.flat() << std::endl;
    }

    return 0;
}
