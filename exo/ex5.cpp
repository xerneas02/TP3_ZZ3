/*************************************************************************
 * Description : Estimation du volume d'une sphère par la méthode        *
 *               de Monte-Carlo, calculée parallèlement.                 *
 *************************************************************************/

#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include "CLHEP/Random/MTwistEngine.h"

// Constantes
#define TIRAGES_PAR_REPLICATION 1000000000 // 1 000 000 000
#define NOMBRE_DE_REPLICATIONS 10

/** --------------------------------------------------------------------- *
 * monte_carlo_volume                                                     *
 *                                                                        *
 * @brief  Calcule une approximation du volume d'une sphère en utilisant  *
 *         la méthode de Monte-Carlo.                                     *
 *                                                                        *
 * @param  generator : moteur de génération aléatoire                     *
 * @param  points : nombre de points à tirer                              *
 *                                                                        *
 * @return Volume approximé de la sphère                                  *
 * --------------------------------------------------------------------- **/
double monte_carlo_volume(CLHEP::MTwistEngine &generator, long long points) 
{
    long long inside = 0;

    for (long long i = 0; i < points; ++i) 
    {
        double x = generator.flat() * 2.0 - 1.0;
        double y = generator.flat() * 2.0 - 1.0;
        double z = generator.flat() * 2.0 - 1.0;

        if (x * x + y * y + z * z <= 1.0) 
        {
            ++inside;
        }
    }

    return 8.0 * static_cast<double>(inside) / points;
}


int main() 
{
    auto start_time = std::chrono::high_resolution_clock::now();

    double volumes[NOMBRE_DE_REPLICATIONS] = {0.0};

    // Calcul parallèle des réplications
    #pragma omp parallel for // Divise la boucle en threads (besoin de -fopenmp pour compiler)
    for (int replication = 0; replication < NOMBRE_DE_REPLICATIONS; ++replication) 
    {
        CLHEP::MTwistEngine generator;

        std::string status_file = "save/status_" + std::to_string(replication) + ".txt";
        generator.restoreStatus(status_file.c_str());

        auto replication_start = std::chrono::high_resolution_clock::now();
        volumes[replication] = monte_carlo_volume(generator, TIRAGES_PAR_REPLICATION);
        auto replication_end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> replication_time = replication_end - replication_start;

        #pragma omp critical
        std::cout << "Réplication " << replication + 1
                  << ": Volume approximé = " << volumes[replication]
                  << " (temps = " << replication_time.count() << " secondes)\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = end_time - start_time;

    std::cout << "\nTemps total de calcul parallèle : " << total_time.count() << " secondes.\n";

    return 0;
}
