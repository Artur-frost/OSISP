#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

const int BUFFER_SIZE = 5; // ������ ���� �������
const int NUM_PROCESSES = 3; // ���������� ���������-�������������

std::vector<int> buffer(BUFFER_SIZE); // ��� �������
std::vector<std::mutex> mutexes(BUFFER_SIZE); // �������� ��� ������� ������
std::vector<std::condition_variable> cvars(BUFFER_SIZE); // �������� ���������� ��� ������� ������

int currentBuffer = 0; // ������� ��������� �����

// ������� ��� ��������-������������
void processData(int id) {
    int data = id + 1;

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mutexes[currentBuffer]); // ����������� ������� ������

            // ����, ���� ����� �� ������ �������� ��� ������
            cvars[currentBuffer].wait(lock, [] { return buffer[currentBuffer] == 0; });

            // ���������� ������ � �����
            buffer[currentBuffer] = data;
            std::cout << "Process " << id << " wrote data: " << data << " into buffer: " << currentBuffer << std::endl;

            // ����������� ����� ��� ������ ������ ���������
            cvars[currentBuffer].notify_one();
        }

        // ��������� � ���������� ������
        currentBuffer = (currentBuffer + 1) % BUFFER_SIZE;
    }
}

int main() {
    std::vector<std::thread> threads;

    // ������� � ��������� ��������-�������������
    for (int i = 0; i < NUM_PROCESSES; i++) {
        threads.emplace_back(processData, i);
    }

    // ������� ���������� ���� ���������
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}


