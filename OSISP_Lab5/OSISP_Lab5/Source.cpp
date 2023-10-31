#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

const int BUFFER_SIZE = 5; // Размер пула буферов
const int NUM_PROCESSES = 3; // Количество процессов-пользователей

std::vector<int> buffer(BUFFER_SIZE); // Пул буферов
std::vector<std::mutex> mutexes(BUFFER_SIZE); // Мьютексы для каждого буфера
std::vector<std::condition_variable> cvars(BUFFER_SIZE); // Условные переменные для каждого буфера

int currentBuffer = 0; // Текущий доступный буфер

// Функция для процесса-пользователя
void processData(int id) {
    int data = id + 1;

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mutexes[currentBuffer]); // Захватываем мьютекс буфера

            // Ждем, пока буфер не станет доступен для записи
            cvars[currentBuffer].wait(lock, [] { return buffer[currentBuffer] == 0; });

            // Записываем данные в буфер
            buffer[currentBuffer] = data;
            std::cout << "Process " << id << " wrote data: " << data << " into buffer: " << currentBuffer << std::endl;

            // Освобождаем буфер для чтения другим процессом
            cvars[currentBuffer].notify_one();
        }

        // Переходим к следующему буферу
        currentBuffer = (currentBuffer + 1) % BUFFER_SIZE;
    }
}

int main() {
    std::vector<std::thread> threads;

    // Создаем и запускаем процессы-пользователей
    for (int i = 0; i < NUM_PROCESSES; i++) {
        threads.emplace_back(processData, i);
    }

    // Ожидаем завершения всех процессов
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}


