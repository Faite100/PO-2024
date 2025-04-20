# Caminho base do CPLEX
CPLEX_DIR = /opt/ibm/ILOG/CPLEX_Studio_Community2212

# Includes
INCLUDES = \
  -I$(CPLEX_DIR)/cplex/include \
  -I$(CPLEX_DIR)/concert/include

# Bibliotecas
LIBDIRS = \
  -L$(CPLEX_DIR)/cplex/lib/x86-64_linux/static_pic \
  -L$(CPLEX_DIR)/concert/lib/x86-64_linux/static_pic

# Nome do executável
TARGET = out

# Fonte
SRC = $(or $(FILE), main.cpp)

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 $(INCLUDES)
LDFLAGS = $(LIBDIRS) -lilocplex -lconcert -lcplex -lm -lpthread -ldl

# Regras
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)