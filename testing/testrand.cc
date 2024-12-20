#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include "CLHEP/Random/MTwistEngine.h"

#define BUFFER_SIZE 1024 // Taille du tampon (nombre d'entiers)

int main ()
{
    CLHEP::MTwistEngine * s = new CLHEP::MTwistEngine();

    int fs;
    double f;
    unsigned int buffer[BUFFER_SIZE]; // Tampon pour stocker les nombres
    unsigned int nbr;

    fs = open("./rngbForMarsagliaTests", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

    if (fs == -1) {
        perror("Erreur lors de l'ouverture du fichier");
        delete s;
        return 1;
    }

    int buffer_index = 0;

    for (int i = 0; i < 3000000; i++) {
        f = s->flat();
        nbr = static_cast<unsigned int>(f * UINT_MAX);

        // Stocker le nombre dans le tampon
        buffer[buffer_index++] = nbr;

        // Si le tampon est plein, écrire son contenu dans le fichier
        if (buffer_index == BUFFER_SIZE) {
            write(fs, buffer, sizeof(buffer));
            buffer_index = 0; // Réinitialiser l'indice
        }
    }

    // Écrire les données restantes dans le tampon (s'il en reste)
    if (buffer_index > 0) {
        write(fs, buffer, buffer_index * sizeof(unsigned int));
    }

    close(fs);
    delete s;

    return 0;
}
