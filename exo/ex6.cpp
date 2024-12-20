#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <climits>
#include <cfloat>
#include "CLHEP/Random/MTwistEngine.h"

const char NUCLEOTIDES[] = {'A', 'C', 'G', 'T'};
const int NUM_NUCLEOTIDES = 4;
const std::string TARGET_SEQUENCE = "AAATTTGCGTTCGA";
const int TARGET_LENGTH = TARGET_SEQUENCE.length();
const int NUM_REPLICATIONS = 10;
const int NUM_THREADS = 8;
const int CHECK_INTERVAL = 1; // N : Vérifier la variable partagé tous les N tours

// Fonction de simulation séquentielle
void simulate_sequential(std::vector<long long> &tries_needed_seq, double &time_seq) {
    auto start_seq = std::chrono::high_resolution_clock::now();

    CLHEP::MTwistEngine generator;
    std::string status_file = "save/status_0.txt"; // Chargement états de départ
    generator.restoreStatus(status_file.c_str());

    for (int replication = 0; replication < NUM_REPLICATIONS; ++replication) {
        long long tries = 0;

        while (true) {
            std::string sequence;
            for (int i = 0; i < TARGET_LENGTH; ++i) {
                int index = generator.flat() * NUM_NUCLEOTIDES;
                char nucleotide = NUCLEOTIDES[index];
                sequence += nucleotide;
            }
            ++tries;

            if (sequence == TARGET_SEQUENCE) {
                tries_needed_seq[replication] = tries;
                std::cout << "Séquentiel - Réplication " << replication + 1 << ": Séquence trouvée après " << tries << " essais.\n";
                break;
            }
        }
    }

    auto end_seq = std::chrono::high_resolution_clock::now();
    time_seq = std::chrono::duration<double>(end_seq - start_seq).count();
}

// Fonction de simulation parallèle
void simulate_parallel(std::vector<long long> &tries_needed_par, std::vector<double> &time_needed_par, double &time_par) {
    auto start_par = std::chrono::high_resolution_clock::now();

    for (int replication = 0; replication < NUM_REPLICATIONS; ++replication) {
        std::vector<long long> tries_per_thread(NUM_THREADS, 0);
        long long min_tries = LLONG_MAX; // Variable partagée indiquant si la séquence a été trouvée
        auto start_thread = std::chrono::high_resolution_clock::now();

        #pragma omp parallel num_threads(NUM_THREADS) shared(min_tries)
        {
            int thread_id = omp_get_thread_num();

            CLHEP::MTwistEngine generator;
            int status_index = replication * NUM_THREADS + thread_id;
            status_index = status_index % 400; // Assure que l'index est dans la plage [0, 399] mais 10*8 < 400 donc ne devrais pas servir

            std::string status_file = "save/status_" + std::to_string(status_index) + ".txt";
            generator.restoreStatus(status_file.c_str());

            long long local_tries = 0;

            while (true) {
                // Vérifier si un autre thread a trouvé la séquence tous les CHECK_INTERVAL itérations
                if (local_tries % CHECK_INTERVAL == 0) {
                    long long local_min_tries;
                    #pragma omp atomic read
                    local_min_tries = min_tries;
                    if (local_tries > local_min_tries) {
                        break;
                    }
                }

                std::string sequence;
                for (int i = 0; i < TARGET_LENGTH; ++i) {
                    int index = generator.flat() * NUM_NUCLEOTIDES;
                    char nucleotide = NUCLEOTIDES[index];
                    sequence += nucleotide;
                }
                ++local_tries;

                if (sequence == TARGET_SEQUENCE) {
                    tries_per_thread[thread_id] = local_tries;

                    // Mettre à jour la variable partagé
                    #pragma omp atomic write
                    min_tries = local_tries;

                    #pragma omp critical
                    {
                        std::cout << "Parallèle - Réplication " << replication + 1 << ", Thread " << thread_id
                                  << ": Séquence trouvée après " << local_tries << "\n";
                    }
                    break;
                }
            }
        }

        auto end_thread = std::chrono::high_resolution_clock::now();
        // Trouver le minimum des nombres d'essais parmi les threads assure aussi la replicabilité dut à l'ordre de recherche
        min_tries = LLONG_MAX;
        double min_time = std::chrono::duration<double>(end_thread - start_thread).count();
        for (int i = 0; i < NUM_THREADS; ++i) {
            if (tries_per_thread[i] > 0 && tries_per_thread[i] < min_tries) {
                min_tries = tries_per_thread[i];
            }
        }

        // Enregistrer les résultats pour cette réplication
        tries_needed_par[replication] = min_tries;
        time_needed_par[replication] = min_time;
    }

    auto end_par = std::chrono::high_resolution_clock::now();
    time_par = std::chrono::duration<double>(end_par - start_par).count();
}

int main() {
    std::vector<long long> tries_needed_seq(NUM_REPLICATIONS);
    std::vector<long long> tries_needed_par(NUM_REPLICATIONS);
    std::vector<double> time_needed_par(NUM_REPLICATIONS); // Temps du thread le plus rapide
    double time_seq = 0.0, time_par = 0.0;

    // Simulation séquentielle
    simulate_sequential(tries_needed_seq, time_seq);

    // Calcul de la moyenne et de l'écart-type pour la simulation séquentielle
    double sum_seq = 0.0;
    for (long long tries : tries_needed_seq) {
        sum_seq += tries;
    }
    double mean_seq = sum_seq / NUM_REPLICATIONS;

    double sq_diff_sum_seq = 0.0;
    for (long long tries : tries_needed_seq) {
        sq_diff_sum_seq += (tries - mean_seq) * (tries - mean_seq);
    }
    double variance_seq = sq_diff_sum_seq / (NUM_REPLICATIONS - 1);
    double stddev_seq = std::sqrt(variance_seq);

    // Simulation parallèle
    simulate_parallel(tries_needed_par, time_needed_par, time_par);

    // Calcul de la moyenne et de l'écart-type pour la simulation parallèle
    double sum_par = 0.0, sum_time_par = 0.0;
    for (int i = 0; i < NUM_REPLICATIONS; ++i) {
        sum_par += tries_needed_par[i];
        sum_time_par += time_needed_par[i];
    }
    double mean_par = sum_par / NUM_REPLICATIONS;
    double mean_time_par = sum_time_par / NUM_REPLICATIONS;

    double sq_diff_sum_par = 0.0, sq_diff_time_par = 0.0;
    for (int i = 0; i < NUM_REPLICATIONS; ++i) {
        sq_diff_sum_par += (tries_needed_par[i] - mean_par) * (tries_needed_par[i] - mean_par);
        sq_diff_time_par += (time_needed_par[i] - mean_time_par) * (time_needed_par[i] - mean_time_par);
    }
    double variance_par = sq_diff_sum_par / (NUM_REPLICATIONS - 1);
    double variance_time_par = sq_diff_time_par / (NUM_REPLICATIONS - 1);
    double stddev_par = std::sqrt(variance_par);
    double stddev_time_par = std::sqrt(variance_time_par);

    // Affichage des résultats
    std::cout << "\n--- Résultats Séquentiels ---\n";
    std::cout << "Nombre moyen d'essais : " << mean_seq << "\n";
    std::cout << "Écart-type : " << stddev_seq << "\n";
    std::cout << "Temps d'execution : " << time_seq << " secondes\n";

    std::cout << "\n--- Résultats Parallèles ---\n";
    std::cout << "Nombre moyen d'essais : " << mean_par << "\n";
    std::cout << "Écart-type : " << stddev_par << "\n";
    std::cout << "Temps moyen : " << mean_time_par << " secondes\n";
    std::cout << "Écart-type : " << stddev_time_par << "\n";
    std::cout << "Temps d'exécution : " << time_par << " secondes\n";

    // Comparaison des temps
    std::cout << "\n--- Comparaison des Temps ---\n";
    std::cout << "Temps séquentiel total : " << time_seq << " secondes\n";
    std::cout << "Temps parallèle total : " << time_par << " secondes\n";

    return 0;
}
