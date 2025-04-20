/*
  Problema de Localização de Depósitos

  Dado um conjunto de centros onde depósitos podem ser construídos (N),
  e um conjunto de clientes com demandas (M), o objetivo é:
  Minimizar os custos totais de instalação e atendimento, onde:

    - fi é o custo fixo de instalar o depósito i
    - cij é o custo de atender o cliente j a partir do depósito i

  Cada cliente deve ser atendido por exatamente um depósito,
  e um depósito só pode atender se estiver instalado.
*/

#include <ilcplex/ilocplex.h>
#include <vector>
#include <iostream>

ILOSTLBEGIN;

int main() {
    IloEnv env;
    IloModel model(env);

    int n = 3; // Número de depósitos possíveis
    int m = 4; // Número de clientes

    // Custo de instalação de cada depósito
    double f[n] = {100, 150, 120};

    // Custo de atendimento do depósito i ao cliente j
    std::vector<std::vector<double>> c = {
        {20, 104, 11, 325},
        {28, 104, 325, 8},
        {325, 5, 5, 96}
    };

    // Variável binária: y[i] = 1 se depósito i for instalado
    IloIntVarArray y(env, n, 0, 1);

    // Variável binária: x[i][j] = 1 se cliente j for atendido pelo depósito i
    IloArray<IloIntVarArray> x(env, n);
    for (int i = 0; i < n; ++i)
        x[i] = IloIntVarArray(env, m, 0, 1);

    // Função objetivo: minimizar o custo total
    IloExpr custoTotal(env);

    // Soma dos custos de instalação
    for (int i = 0; i < n; ++i)
        custoTotal += f[i] * y[i];

    // Soma dos custos de atendimento
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            custoTotal += c[i][j] * x[i][j];

    model.add(IloMinimize(env, custoTotal));

    // Restrição: cada cliente deve ser atendido por um único depósito
    for (int j = 0; j < m; ++j) {
        IloExpr atendimento(env);
        for (int i = 0; i < n; ++i)
            atendimento += x[i][j];
        model.add(atendimento == 1);
    }

    // Restrição: um cliente só pode ser atendido por um depósito instalado
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < m; ++j)
            model.add(x[i][j] <= y[i]);

    // Resolver o modelo
    IloCplex cplex(model);
    if (!cplex.solve()) {
        std::cout << "Problema não resolvido.\n";
        return 1;
    }

    std::cout << "Problema resolvido com sucesso!\n";
    std::cout << "Custo mínimo total: " << cplex.getObjValue() << "\n\n";

    for (int i = 0; i < n; ++i) {
        if (cplex.getValue(y[i]) > 0.5) {
            std::cout << "Depósito " << i << " instalado.\n";
            std::cout << "  Clientes atendidos: ";
            for (int j = 0; j < m; ++j) {
                if (cplex.getValue(x[i][j]) > 0.5)
                    std::cout << j << " ";
            }
            std::cout << "\n";
        }
    }

    return 0;
}
