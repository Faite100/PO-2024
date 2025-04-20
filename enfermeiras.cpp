/*
  Problema de Escalonamento de Horários das Enfermeiras

  Um hospital precisa planejar os turnos noturnos das enfermeiras durante a semana.
  A demanda diária (de domingo a sábado) é especificada em um vetor.

  Cada enfermeira trabalha por 5 dias consecutivos e depois descansa 3 dias.
  O objetivo é contratar o menor número possível de enfermeiras, garantindo que a
  demanda de cada dia seja atendida.
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

ILOSTLBEGIN;

const std::vector<int> DEMANDAS = {
  5, 6, 4, 7, 3, 4, 6 // Demanda de enfermeiras para os 7 dias da semana
};

int main() {
  IloEnv env;
  IloModel model(env);

  const int dias = DEMANDAS.size();

  // x[i] representa o número de enfermeiras que começam a trabalhar no dia i
  IloIntVarArray x(env, dias, 0, IloInfinity);

  // Função objetivo: minimizar o total de enfermeiras contratadas
  IloExpr totalEnfermeiras(env);
  for (int i = 0; i < dias; i++) {
    totalEnfermeiras += x[i];
  }
  model.add(IloMinimize(env, totalEnfermeiras));

  // Restrições de cobertura da demanda em cada dia
  for (int i = 0; i < dias; i++) {
    IloExpr cobrindo(env);
    
    // Soma as enfermeiras que começaram em dias que as fariam trabalhar no dia i
    // Como o trabalho é por 5 dias consecutivos, consideramos as 5 posições anteriores
    for (int j = 0; j < 5; j++) {
      int diaInicio = (i - j + dias) % dias; // cuidado com o wrap-around da semana
      cobrindo += x[diaInicio];
    }

    model.add(cobrindo >= DEMANDAS[i]);
  }

  // Resolver o modelo
  IloCplex cplex(model);
  if (!cplex.solve()) {
    std::cout << "Nenhuma solução encontrada.\n";
    return 1;
  }

  std::cout << "Problema resolvido com sucesso!\n";
  std::cout << "Número mínimo de enfermeiras contratadas: " << cplex.getObjValue() << "\n\n";

  for (int i = 0; i < dias; i++) {
    std::cout << "Enfermeiras que começam no dia " << i + 1 << ": " << cplex.getValue(x[i]) << "\n";
  }

  return 0;
}
