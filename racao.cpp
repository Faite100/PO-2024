/*
  Problema da Ração

  Uma empresa de alimentos caninos produz 2 tipos de ração:
  – All Mega Giga Suprema (AMGS) e Ração das Estrelas (RE).
  – Para a sua manufatura são utilizadas cereais e carne.
  – O AMGS utiliza 5 kg de cereais e 1 kg de carne e o RE utiliza 4 kg de carne e 2 kg
  de cereais.
  – O preço do AMGS é 20 reais e o do RE é 30 reais
  – 1 kg de carne custa 4 reais e 1 kg de cereal custa 1 reais.
  – Existe a disposição 10.000 kg de carne e 30.000 kg de cereais.
  – Como deve ser a produção da empresa para ganhar o máximo de dinheiro?
*/

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN;

int main() {
  IloEnv env;

  IloModel model(env);

  // Variáveis de decisão: 
  // qtd_amgs: quantidade de AMGS produzida
  // qtd_re: quantidade de RE produzida
  IloNumVar qtd_amgs(env, 0, IloInfinity, ILOFLOAT);  // Quantidade de AMGS não pode ser negativa
  IloNumVar qtd_re(env, 0, IloInfinity, ILOFLOAT);    // Quantidade de RE não pode ser negativa

  // Função Objetivo: Maximizar o lucro total
  // O lucro de AMGS é 20 reais e o de RE é 30 reais
  IloObjective obj = IloMaximize(env, qtd_amgs * 20 + qtd_re * 30);
  model.add(obj);  // Adiciona a função objetivo ao modelo

  // Restrições:
  // Cada tipo de ração usa uma certa quantidade de cereais e carne
  // Restrição de carne: AMGS usa 1 kg de carne e RE usa 4 kg
  model.add(qtd_amgs * 1 + qtd_re * 4 <= 10000);  // Máximo de 10.000 kg de carne disponível

  // Restrição de cereal: AMGS usa 5 kg de cereais e RE usa 2 kg
  model.add(qtd_amgs * 5 + qtd_re * 2 <= 30000);  // Máximo de 30.000 kg de cereais disponíveis

  // Resolver o modelo com CPLEX
  IloCplex cplex(model);

  // Tenta resolver o problema
  IloBool result = cplex.solve();

  // Exibe se o problema foi resolvido ou não
  result ? printf("Problema resolvido!\n") : printf("Problema não resolvido\n");

  // Se não houver solução, o programa termina
  if (!result) {
    printf("Nenhuma solução encontrada\n");
    return 1;
  } 

  // Exibe o valor ótimo (lucro máximo)
  printf("Valor ótimo: %f\n", cplex.getObjValue());

  // Exibe a quantidade de AMGS e RE produzidas
  printf("Qtd AMGS: %f\n", cplex.getValue(qtd_amgs));
  printf("Qtd RE: %f\n", cplex.getValue(qtd_re));

  // Exibe a quantidade total de carne utilizada
  printf("Qtd Carne: %f\n", cplex.getValue(qtd_amgs) * 1 + cplex.getValue(qtd_re) * 4);

  // Exibe a quantidade total de cereal utilizado
  printf("Qtd Cereal: %f\n", cplex.getValue(qtd_amgs) * 5 + cplex.getValue(qtd_re) * 2);

  // Exibe o custo total da carne e cereais utilizados
  printf("Custo: %f\n", cplex.getValue(qtd_amgs) * 1 + cplex.getValue(qtd_re) * 4 + cplex.getValue(qtd_amgs) * 5 + cplex.getValue(qtd_re) * 2);

  // Exibe a receita total gerada pela venda das rações
  printf("Receita: %f\n", cplex.getValue(qtd_amgs) * 20 + cplex.getValue(qtd_re) * 30);

  // Exibe o custo das matérias-primas (carne e cereais)
  printf("Custo das matérias-primas: %f\n", cplex.getValue(qtd_amgs) * 5 + cplex.getValue(qtd_re) * 4);

  return 0;
}
