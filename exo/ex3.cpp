#include <iostream>
#include <fstream>
#include <string>
#include "CLHEP/Random/MTwistEngine.h"

int main() 
{
    CLHEP::MTwistEngine generator;

    const long long tirages_par_sauvegarde = 2000000000LL;
    const int nombre_de_statuts = 400;

    for (int i = 0; i < nombre_de_statuts; ++i) 
    {
        for (long long j = 0; j < tirages_par_sauvegarde; ++j) 
        {
            generator.flat();
        }

        std::string filename = "save/status_" + std::to_string(i) + ".txt";
        generator.saveStatus(filename.c_str());
        std::cout << "Statut sauvegardé dans " << filename << " après " 
                  << (i + 1) * tirages_par_sauvegarde << " tirages.\n";
    }

    return 0;
}
