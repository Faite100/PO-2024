#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <thread>
#include <chrono>
#include <future>
#include <stdexcept>
#include <atomic>

void print_items(const std::vector<int>& items);
std::vector<int> permute(const std::vector<int>& initial);
std::vector<int> bin_packing_ff(std::vector<int> items, int capacity);
size_t gen_random_index(size_t size);
std::vector<std::vector<int>> fitness_first_fit(const std::vector<int>& items, int capacity);

std::atomic<bool> stop_execution(false);
std::vector<int> current;

int main(int argc, char* argv[]) {
    std::cout << "Algoritmo de Bin Packing com busca local" << std::endl;

    if (argc < 2) {
        std::cerr << "Uso: " << argv[0] << " <tempo_maximo_execucao (s)>" << std::endl;
        return 1;
    }

    int time_limit = std::stoi(argv[1]);
    int capacity, n;

    std::cin >> capacity >> n;
    std::vector<int> items(n);

    for (int& item : items)
        std::cin >> item;

    current = permute(items);

    // Lança a busca local em paralelo
    auto future_result = std::async(std::launch::async, bin_packing_ff, items, capacity);

    try {
        auto status = future_result.wait_for(std::chrono::seconds(time_limit));
        if (status == std::future_status::ready) {
            std::cout << "Resultado encontrado antes do tempo limite!" << std::endl;
        } else {
            stop_execution = true;
            std::cout << "Tempo limite excedido!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exceção: " << e.what() << std::endl;
    }

    auto bins = fitness_first_fit(current, capacity);

    for (size_t i = 0; i < bins.size(); ++i) {
        std::cout << "Bin " << i + 1 << ": ";
        print_items(bins[i]);
    }

    std::cout << "Número de bins utilizadas: " << bins.size() << std::endl;
    return 0;
}

// Algoritmo de busca local usando permutação de pares
std::vector<int> bin_packing_ff(std::vector<int> items, int capacity) {
    int n = items.size();
    int best_fitness = fitness_first_fit(items, capacity).size();

    while (!stop_execution) {
        int k = std::min(100, n);
        std::vector<std::vector<int>> neighbors(k);

        for (int i = 0; i < k; ++i) {
            int a = gen_random_index(n), b;
            do {
                b = gen_random_index(n);
            } while (a == b);

            auto neighbor = items;
            std::swap(neighbor[a], neighbor[b]);
            neighbors[i] = neighbor;
        }

        for (const auto& neighbor : neighbors) {
            int fit = fitness_first_fit(neighbor, capacity).size();
            if (fit < best_fitness) {
                current = neighbor;
                best_fitness = fit;
            }
        }
    }

    return current;
}

// Função de avaliação usando First Fit
std::vector<std::vector<int>> fitness_first_fit(const std::vector<int>& items, int capacity) {
    std::vector<std::vector<int>> bins;

    for (int item : items) {
        bool placed = false;
        for (auto& bin : bins) {
            int sum = std::accumulate(bin.begin(), bin.end(), 0);
            if (sum + item <= capacity) {
                bin.push_back(item);
                placed = true;
                break;
            }
        }

        if (!placed)
            bins.push_back({item});
    }

    return bins;
}

// Gera índice aleatório para troca
size_t gen_random_index(size_t size) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, size - 1);
    return distrib(gen);
}

// Imprime itens da bin formatadamente
void print_items(const std::vector<int>& items) {
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << items[i];
        if (i < items.size() - 1)
            std::cout << ", ";
    }
    std::cout << '\n';
}

// Retorna uma permutação aleatória dos itens
std::vector<int> permute(const std::vector<int>& initial) {
    auto permutation = initial;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(permutation.begin(), permutation.end(), gen);
    return permutation;
}
