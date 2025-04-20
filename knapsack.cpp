/*
– Dado um conjunto de n itens e um inteiro W que representa a capacidade da 
mochila.
– Cada item i possui um valor vi e um peso wi. 
– Determinar o subconjuntos de itens que maximizam o somatório dos valores 
respeitando a capacidade de peso da mochila.
*/

#include <ilcplex/ilocplex.h>
#include <vector>

ILOSTLBEGIN;

int main() {
    IloEnv env;
    IloModel model(env);

    // Definição do número de itens (n) e a capacidade máxima da mochila (W)
    int n = 20;  // Número de itens
    int W = 878; // Capacidade máxima da mochila

    // Vetores contendo o peso (w) e valor (v) de cada item
    vector<double> w = {92, 4, 43, 83, 84, 68, 92, 82, 6, 44, 32, 18, 56, 83, 25, 96, 70, 48, 14, 58};
    vector<double> v = {44, 46, 90, 72, 91, 40, 75, 35, 8, 54, 78, 40, 77, 15, 61, 17, 75, 29, 75, 63};

    // Declaração das variáveis binárias x[i], que indicam se o item i é incluído na mochila
    IloIntVarArray x(env, n, 0, 1); // 0 ou 1, indicando a inclusão ou não do item na solução

    // Função Objetivo: Maximizar o valor total dos itens incluídos na mochila
    IloExpr somatorioObj(env);
    for (int i = 0; i < n; ++i) {
        somatorioObj += v[i] * x[i];  // Cada valor dos itens é multiplicado pela sua variável binária
    }

    // Define a função objetivo no modelo
    IloObjective obj = IloMaximize(env, somatorioObj);
    model.add(obj);

    // Restrição de capacidade: a soma dos pesos dos itens selecionados não pode ultrapassar a capacidade da mochila
    IloExpr somatorioRes(env);
    for (int i = 0; i < n; ++i) {
        somatorioRes += w[i] * x[i];  // Cada peso dos itens é multiplicado pela sua variável binária
    }
    model.add(somatorioRes <= W);  // Restrição de capacidade da mochila

    // Resolver o modelo usando o solver CPLEX
    IloCplex cplex(model);
    IloBool result = cplex.solve();  // Resolve o modelo otimizado

    // Exibe se o problema foi resolvido ou não
    result ? printf("Problema resolvido!\n") : printf("Problema não resolvido\n");

    // Exibe o valor ótimo da função objetivo (valor máximo dos itens selecionados)
    printf("Valor ótimo: %.2f\n", cplex.getObjValue());

    // Exibe os itens que foram selecionados na solução ótima, juntamente com seus valores e pesos
    printf("Itens selecionados:\n");

    float pesoTotal = 0.0;
    for (int i = 0; i < n; ++i) {
        if (cplex.getValue(x[i]) > 0) { // Verifica se o item foi selecionado (x[i] == 1)
            printf("Item %d - valor: %.2f, peso: %.2f\n", i, v[i], w[i]);
            pesoTotal += w[i];  // Atualiza o peso total dos itens selecionados
        }
    }

    // Exibe o peso total dos itens selecionados
    printf("Peso total: %.2f\n", pesoTotal);

    return 0;
}
