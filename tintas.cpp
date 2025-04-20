
/*
  Uma empresa de tintas produz 2 tipos de tinta: a de secagem rápida (SR) e a de
  secagem normal (SN). Ambas usam os mesmos componente, variando apenas a
  proporção. Os componentes são: componente de secagem (SEC) e o componente
  de cor (COR). A empresa tem a disposição para a compra duas misturas para
  compor as tintas:

  – Solução A (SolA) com 30% de SEC e 70% de COR a R$ 1,5 o litro
  – Solução B (Solb) com 60% de SEC e 40% de COR a R$ 1,0 o litro.

  Além disso, a empresa tem a disposição também:
  – Comp. SEC a R$ 4,0 o litro
  – Comp. COR a R$ 6,0 o litro

  Sabendo que para compor SR temos que ter no mínimo 25% de SEC e no mínimo
  50% de COR. Já para SN temos que ter no mínimo de 20% de SER e 50% de COR.
  Qual a quantidade de produtos (SolA, SolB, SEC, COR) a serem comprados para se
  produzir 1000 litro de SR e 250 litros de SN com menor custo?
*/
#include <ilcplex/ilocplex.h>
#include <vector>

ILOSTLBEGIN;

enum TINTAS {
  SR,  // Secagem Rápida
  SN   // Secagem Normal
};

const std::vector<std::string> TINTAS_NOMES = {
  "Secagem Rápida",  // Nome da tinta SR
  "Secagem Normal",  // Nome da tinta SN
};

enum COMPONENTES {
  SOL_A,  // Solução A
  SOL_B,  // Solução B
  SEC,    // Componente Secante
  COR     // Componente Cor
};

const std::vector<std::string> COMPONENTES_NOMES = {
  "Solução A",      // Nome da solução A
  "Solução B",      // Nome da solução B
  "Componente Secante",  // Nome do componente secante (SEC)
  "Componente Cor"      // Nome do componente cor (COR)
};

int main() {
  IloEnv env;
  IloModel model(env);

  // Variáveis de decisão:
  // x[0][0] até x[0][3] são as variáveis para a tinta de secagem rápida (SR)
  // x[1][0] até x[1][3] são as variáveis para a tinta de secagem normal (SN)
  IloArray<IloIntVarArray> x(env, 2);  // 2 tipos de tinta (SR e SN)

  // Para cada tinta (SR e SN), temos 4 componentes (SolA, SolB, SEC, COR)
  for(unsigned i = 0; i < 2; i++) {
    x[i] = IloIntVarArray(env, 4, 0, IloInfinity);  // Quantidades dos componentes
  }

  // Função objetivo: Minimizar o custo total
  IloExpr obj_expr(env);
  
  // Custo de SolA: R$ 1,5 por litro
  obj_expr += 1.5 * (x[SR][SOL_A] + x[SN][SOL_A]);
  
  // Custo de SolB: R$ 1,0 por litro
  obj_expr += 1 * (x[SR][SOL_B] + x[SN][SOL_B]);
  
  // Custo de SEC: R$ 4,0 por litro
  obj_expr += 4 * (x[SR][SEC] + x[SN][SEC]);
  
  // Custo de COR: R$ 6,0 por litro
  obj_expr += 6 * (x[SR][COR] + x[SN][COR]);

  // Define a função objetivo como minimização do custo
  IloObjective obj = IloMinimize(env, obj_expr);
  model.add(obj);  // Adiciona ao modelo

  // Restrição de quantidade de tinta SR (1000 litros)
  model.add((x[SR][SOL_A] + x[SR][SOL_B] + x[SR][COR] + x[SR][SEC]) == 1000);

  // Restrição de quantidade de tinta SN (250 litros)
  model.add((x[SN][SOL_A] + x[SN][SOL_B] + x[SN][COR] + x[SN][SEC]) == 250);

  // Restrições de quantidade mínima de SEC (25% para SR e 20% para SN)
  model.add(( 0.3 * x[SR][SOL_A] + 0.6 * x[SR][SOL_B] + x[SR][SEC]) >= 250);  // Para SR
  model.add(( 0.3 * x[SN][SOL_A] + 0.6 * x[SN][SOL_B] + x[SN][SEC]) >= 50);   // Para SN

  // Restrições de quantidade mínima de COR (50% para SR e 50% para SN)
  model.add(( 0.7 * x[SR][SOL_A] + 0.4 * x[SR][SOL_B] + x[SR][COR]) >= 500);   // Para SR
  model.add(( 0.7 * x[SN][SOL_A] + 0.4 * x[SN][SOL_B] + x[SN][COR]) >= 125);  // Para SN

  // Resolver o modelo usando CPLEX
  IloCplex cplex(model);
  IloBool result = cplex.solve();  // Tenta resolver o problema

  // Se não houver solução, exibe uma mensagem
  if (!result) {
    printf("Nenhuma solução encontrada\n");
    return 1;
  }

  // Se o problema for resolvido, exibe o valor ótimo (custo mínimo)
  printf("Problema resolvido!\n");
  printf("Custo mínimo: %f\n", cplex.getObjValue());

  // Exibe a quantidade de cada componente para cada tinta
  for (unsigned i = 0; i < 2; i++) {
    printf("Tinta %s:\n", TINTAS_NOMES[i].c_str());
    for (unsigned j = 0; j < 4; j++) {
      printf("  %s: %f\n", COMPONENTES_NOMES[j].c_str(), cplex.getValue(x[i][j]));
    }
  }

  return 0;
}
