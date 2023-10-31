#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

const int NUM_PHILOSOPHERS = 5;

std::mutex forks[NUM_PHILOSOPHERS];
std::condition_variable cond_vars[NUM_PHILOSOPHERS];

void philosopher(int id) {
    int left_fork = id;
    int right_fork = (id + 1) % NUM_PHILOSOPHERS;

    while (true) {
        std::unique_lock<std::mutex> left_lock(forks[left_fork]);
        std::unique_lock<std::mutex> right_lock(forks[right_fork]);

        std::cout << "Philosopher " << id << " is eating." << std::endl;

        // Simulate eating
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        right_lock.unlock();
        left_lock.unlock();

        std::cout << "Philosopher " << id << " is thinking." << std::endl;

        // Simulate thinking
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {
    std::vector<std::thread> philosophers;

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        philosophers.emplace_back(philosopher, i);
    }

    for (auto& philosopher : philosophers) {
        philosopher.join();
    }

    return 0;
}
