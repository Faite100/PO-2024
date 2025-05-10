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
#include <iomanip>

void print_items(const std::vector<float>& items);
std::vector<float> permute(const std::vector<float>& initial);
std::vector<float> bin_packing_ff(std::vector<float> items, float capacity);
size_t gen_random_index(size_t size);
std::vector<std::vector<float>> fitness_first_fit(const std::vector<float>& items, float capacity);

std::atomic<bool> stop_execution(false);
std::vector<float> current;

int main(int argc, char* argv[]) {
    std::cout << "Bin Packing Algorithm with local search (floating-point version)\n";
    std::cout << std::fixed << std::setprecision(2);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <max_execution_time (s)>\n";
        return 1;
    }

    int time_limit = std::stoi(argv[1]);
    const float capacity = 1.0f;  // Fixed bin capacity
    int n;
    
    std::cin >> n;
    std::vector<float> items(n);
    
    for (float& item : items) {
        std::cin >> item;
        if (item <= 0.0f || item > 1.0f) {
            std::cerr << "Error: All items must be between 0 and 1\n";
            return 1;
        }
    }

    current = permute(items);

    auto future_result = std::async(std::launch::async, bin_packing_ff, items, capacity);

    try {
        auto status = future_result.wait_for(std::chrono::seconds(time_limit));
        if (status == std::future_status::ready) {
            std::cout << "Solution found before time limit!\n";
        } else {
            stop_execution = true;
            std::cout << "Time limit exceeded!\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }

    auto bins = fitness_first_fit(current, capacity);

    for (size_t i = 0; i < bins.size(); ++i) {
        float sum = std::accumulate(bins[i].begin(), bins[i].end(), 0.0f);
        std::cout << "Bin " << i + 1 << " (sum: " << sum << "): ";
        print_items(bins[i]);
    }

    std::cout << "Number of bins used: " << bins.size() << '\n';
    return 0;
}

std::vector<float> bin_packing_ff(std::vector<float> items, float capacity) {
    int n = items.size();
    int best_fitness = fitness_first_fit(items, capacity).size();

    while (!stop_execution) {
        int k = std::min(100, n);
        std::vector<std::vector<float>> neighbors(k);

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
                // Early exit if perfect solution found (unlikely for floating-point)
                if (best_fitness == 1) break;
            }
        }
    }

    return current;
}

std::vector<std::vector<float>> fitness_first_fit(const std::vector<float>& items, float capacity) {
    std::vector<std::vector<float>> bins;
    const float epsilon = 1e-5f;  // For floating-point comparisons

    for (float item : items) {
        bool placed = false;
        for (auto& bin : bins) {
            float sum = std::accumulate(bin.begin(), bin.end(), 0.0f);
            if (sum + item <= capacity + epsilon) {
                bin.push_back(item);
                placed = true;
                break;
            }
        }

        if (!placed) {
            bins.push_back({item});
        }
    }

    return bins;
}

size_t gen_random_index(size_t size) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> distrib(0, size - 1);
    return distrib(gen);
}

void print_items(const std::vector<float>& items) {
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << items[i];
        if (i < items.size() - 1)
            std::cout << ", ";
    }
    std::cout << '\n';
}

std::vector<float> permute(const std::vector<float>& initial) {
    auto permutation = initial;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(permutation.begin(), permutation.end(), gen);
    return permutation;
}