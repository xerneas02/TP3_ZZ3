# Chemins vers les répertoires de la bibliothèque CLHEP
INCLUDE_DIR = /mnt/d/Programation/c++/simu/TP3_ZZ3/CLHEP/Random/include/
LIB_DIR = /mnt/d/Programation/c++/simu/TP3_ZZ3/CLHEP/Random/lib

# Nom de la bibliothèque à lier
LIBS = -lCLHEP-Random-2.1.0.0

# Flags pour le compilateur
CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -I$(INCLUDE_DIR)
LDFLAGS = -L$(LIB_DIR) $(LIBS)

# Fichiers sources et exécutables
SRC = ex6.cpp
EXE = ex6

# Règles Makefile
all: $(EXE)
#	@echo "Exporting LD_LIBRARY_PATH..."
#	@export LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) -fopenmp

clean:
	rm -f $(EXE)
