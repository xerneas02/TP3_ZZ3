#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>
#include "CLHEP/Random/MTwistEngine.h"

const long long TIRAGES_PAR_REPLICATION = 1000000000; // 1 milliard de points
const int NOMBRE_DE_REPLICATIONS = 10;

double monte_carlo_volume(CLHEP::MTwistEngine &generator, long long points) {
    long long inside = 0;

    for (long long i = 0; i < points; ++i) {
        double x = generator.flat() * 2.0 - 1.0; // Entre -1 et 1
        double y = generator.flat() * 2.0 - 1.0; 
        double z = generator.flat() * 2.0 - 1.0; 

        if (x * x + y * y + z * z <= 1.0) {
            ++inside;
        }
    }

    // Volume de la sphère dans un cube de côté 2 (donc volume total 8)
    return 8.0 * static_cast<double>(inside) / points;
}

int main() {
    CLHEP::MTwistEngine generator;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int replication = 0; replication < NOMBRE_DE_REPLICATIONS; ++replication) {
        std::string status_file = "save/status_" + std::to_string(replication) + ".txt";
        generator.restoreStatus(status_file.c_str());
        std::cout << "Statut restauré depuis " << status_file << ".\n";

        auto replication_start = std::chrono::high_resolution_clock::now();
        double volume = monte_carlo_volume(generator, TIRAGES_PAR_REPLICATION);
        auto replication_end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> replication_time = replication_end - replication_start;
        std::cout << "Réplication " << replication + 1 << ": Volume approximé = " << volume
                  << " (temps = " << replication_time.count() << " secondes)\n";
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = end_time - start_time;
    std::cout << "\nTemps total de calcul : " << total_time.count() << " secondes.\n";

    return 0;
}
