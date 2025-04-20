/*
– Uma fábrica de latinhas possui 4 padrões de impressão em folhas de metal
(existem 2 tipos de folhas demetal diferentes).
– A fábrica possui 200 folhas de metal de tam 1 e 90 de tam 2.
– Cada latinhas é vendida a 50 u.
– Cada corpo não utilizado possui um custo de estocagem de 50 u e cada tampa 
custa 3 u.
– Quantas impressões de cada padrão devem ser feitas para maximizar o lucro?
*/
#include <ilcplex/ilocplex.h>
#include <vector>

ILOSTLBEGIN;

int main() {
    IloEnv env;
    IloModel model(env);

    // Declaração das variáveis de decisão
    // x[0], x[1], x[2], x[3]: Quantidade de impressões de cada padrão
    // y: Quantidade de latinhas produzidas
    IloIntVarArray x(env, 4, 0, IloIntMax); // Quantidades de impressões de cada padrão (não negativas)
    IloIntVar y(env, 0, IloIntMax);         // Quantidade de latinhas produzidas (não negativas)

    // Função Objetivo: Maximizar o lucro
    // A função objetivo é composta por 3 partes:
    // 1. Lucro de latinhas vendidas (50 u por latinha)
    // 2. Custo de estocagem dos corpos não utilizados
    // 3. Custo das tampas
    IloObjective obj = IloMaximize(env, 
        50 * y - 5 * (x[0] + 2 * x[1] + 4 * x[3] - y) - 3 * (7 * x[0] + 3 * x[1] + 9 * x[2] + 4 * x[3] - 2 * y)
    );

    // Adiciona a função objetivo ao modelo
    model.add(obj);

    // Restrições
    // Restrição de folhas de metal do tipo 1 (máximo de 200 folhas)
    model.add(x[0] + x[2] + x[3] <= 200);

    // Restrição de folhas de metal do tipo 2 (máximo de 90 folhas)
    model.add(x[1] <= 90);

    // Restrição: quantidade de latinhas produzidas (y) deve ser menor ou igual à quantidade de impressões feitas nos padrões
    model.add(y <= x[0] + 2 * x[1] + 4 * x[3]);

    // Restrição de latinhas produzidas com base nas tampas e corpos
    model.add(y <= (7 * x[0] + 3 * x[1] + 9 * x[2] + 4 * x[3])/2);

    // Resolve o modelo utilizando o solver CPLEX
    IloCplex cplex(model);
    IloBool result = cplex.solve(); // Resolve o problema

    // Exibe o resultado do problema
    result ? printf("Problema resolvido!\n") : printf("Problema não resolvido\n");
    
    // Exibe o valor ótimo da função objetivo (lucro máximo)
    printf("Valor ótimo: %.0f\n", cplex.getObjValue());

    // Exibe a quantidade de impressões feitas para cada padrão e a quantidade de latinhas produzidas
    printf("x1 = %.0f, x2 = %.0f, x3 = %.0f, x4 = %.0f\n", 
        cplex.getValue(x[0]), cplex.getValue(x[1]), cplex.getValue(x[2]), cplex.getValue(x[3]));
    printf("y = %.0f\n", cplex.getValue(y));

    return 0;
}
