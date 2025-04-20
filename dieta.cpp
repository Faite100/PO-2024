/*
  Problema da dieta

  Uma pessoa precisa ingerir quantidades mínimas de vitaminas A e C,
  usando combinações de 6 ingredientes. O objetivo é minimizar o custo total
  do composto alimentar respeitando essas necessidades nutricionais.

  Vitaminas por ingrediente:
              | Vit A | Vit C
  Ingred. 0   |   1   |   0
  Ingred. 1   |   0   |   1
  Ingred. 2   |   2   |   3
  Ingred. 3   |   2   |   1
  Ingred. 4   |   1   |   3
  Ingred. 5   |   2   |   2

  Mínimo necessário:
    Vitamina A: 9
    Vitamina C: 19

  Custo por ingrediente: 35, 30, 60, 50, 27, 22
*/

#include <ilcplex/ilocplex.h>
#include <iostream>

int main() {
  IloEnv env;
  IloModel model(env);

  const int num_ingredientes = 6;

  // Variáveis de decisão: quantidade de cada ingrediente no composto
  IloFloatVarArray x(env, num_ingredientes, 0, IloInfinity);

  // Custo de cada ingrediente
  const int custo[] = {35, 30, 60, 50, 27, 22};

  // Adiciona a função objetivo: minimizar custo total
  IloExpr custo_total(env);
  for (int i = 0; i < num_ingredientes; ++i) {
    custo_total += custo[i] * x[i];
  }
  model.add(IloMinimize(env, custo_total));

  // Matriz de vitaminas: linha = tipo de vitamina, coluna = ingrediente
  int vitamina[2][6] = {
    {1, 0, 2, 2, 1, 2}, // Vitamina A
    {0, 1, 3, 1, 3, 2}  // Vitamina C
  };

  // Quantidade mínima necessária de cada vitamina
  int minimo[2] = {9, 19};

  // Adiciona restrições nutricionais
  for (int v = 0; v < 2; ++v) {
    IloExpr quantidade(env);
    for (int i = 0; i < num_ingredientes; ++i) {
      quantidade += vitamina[v][i] * x[i];
    }
    model.add(quantidade >= minimo[v]);
  }

  // Resolve o problema
  IloCplex cplex(model);

  if (!cplex.solve()) {
    std::cout << "Nenhuma solução encontrada.\n";
    return 1;
  }

  std::cout << "Problema resolvido com sucesso!\n\n";
  std::cout << "Custo total ótimo: " << cplex.getObjValue() << "\n";

  for (int i = 0; i < num_ingredientes; ++i) {
    std::cout << "Ingrediente " << i << ": " << cplex.getValue(x[i]) << "\n";
  }

  // Cálculo das vitaminas obtidas na solução
  double total_vitA = 0, total_vitC = 0;
  for (int i = 0; i < num_ingredientes; ++i) {
    total_vitA += vitamina[0][i] * cplex.getValue(x[i]);
    total_vitC += vitamina[1][i] * cplex.getValue(x[i]);
  }

  std::cout << "\nTotal de vitamina A: " << total_vitA << "\n";
  std::cout << "Total de vitamina C: " << total_vitC << "\n";

  return 0;
}
