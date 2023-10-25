#include <iostream>
#include <fstream>
#include <string>
#include <future>

std::string read_file_content(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file) {
        throw std::runtime_error("Failed to open the file");
    }

    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    return content;
}

void process_file_content(const std::string& file_path) {
    std::cout << "Processing file: " << file_path << std::endl;

    try {
        // Асинхронно читаем содержимое файла
        auto future = std::async(std::launch::async, read_file_content, file_path);

        // Другие асинхронные операции могут быть выполнены в это время

        // Получаем результат из будущего (future)
        std::string content = future.get();

        // Обрабатываем содержимое файла (в данном примере просто выводим его)
        std::cout << "File content:\n" << content << std::endl;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
}

int main() {
    std::string file_path("C:\\Users\\MyPC\\Documents\\OSISP4.txt");
    process_file_content(file_path);

    return 0;
}

