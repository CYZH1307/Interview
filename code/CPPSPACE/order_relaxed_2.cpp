#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <chrono>


// alignas(64) 优化策略
alignas(64) std::atomic<int> count_DA{0};
char padding[60];  // Padding to offset y to a different cache line.
alignas(64) std::atomic<int> count_DABC{0}; 

void run_experiment(int N) {
    std::atomic<int> x{0};
    std::atomic<int> y{0};
    std::atomic<int> r1{0}; 
    std::atomic<int> r2{0}; 

    auto thread1_func = [&]() {
        r1 = y.load(std::memory_order_relaxed);  // A
        x.store(r1, std::memory_order_relaxed);  // B
    };

    auto thread2_func = [&]() {
        r2 = x.load(std::memory_order_relaxed);  // C
        y.store(42, std::memory_order_relaxed);  // D
    };

    for (int i = 0; i < N; ++i) {
        x = 0;
        y = 0;

        std::thread thread1(thread1_func);
        std::thread thread2(thread2_func);

        thread1.join();
        thread2.join();

        if (r1 == 42 && r2 == 42) {
            count_DABC++;
        } 
        else if(r1 == 42) count_DA++;
    }
}

int main() {
    constexpr int N = 1e5;
    constexpr int num_threads = 10;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for(int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&] {
            run_experiment(N / num_threads);
        });
    }

    for(auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "count_DA = " << count_DA << "\n";
    std::cout << "Probability of DA = " << static_cast<double>(count_DA) * 100 / N << "%\n";
    std::cout << "count_DABC = " << count_DABC << "\n";
    std::cout << "Probability of DABC = " << static_cast<double>(count_DABC) * 100 / N << "%\n";
    std::cout << "Elapsed time: " << elapsed.count() << "s\n";

    return 0;
}