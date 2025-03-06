#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

class ArraySumCalculator {
private:
    std::vector<int> array;
    size_t numThreads;

    // Разбиение массива на части
    std::vector<std::vector<int>> splitArray(const std::vector<int>& arr, size_t parts) {
        std::vector<std::vector<int>> result;
        size_t size = arr.size();
        size_t partSize = (size + parts - 1) / parts; // Округление вверх

        for (size_t i = 0; i < size; i += partSize) {
            size_t end = std::min(i + partSize, size);
            result.push_back(std::vector<int>(arr.begin() + i, arr.begin() + end));
        }

        return result;
    }

    // Вычисление суммы части массива
    void calculateSum(const std::vector<int>& part, int& result) {
        result = 0;
        for (int num : part) {
            result += num;
        }
    }

public:
    ArraySumCalculator(const std::vector<int>& arr, size_t threads) : array(arr), numThreads(threads) {}

    // Вычисление суммы с использованием нескольких потоков
    int calculateParallelSum() {
        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::vector<int>> parts = splitArray(array, numThreads);
        std::vector<int> results(numThreads, 0);
        std::vector<std::thread> threads;

        for (size_t i = 0; i < parts.size(); ++i) {
            threads.push_back(std::thread(&ArraySumCalculator::calculateSum, this, parts[i], std::ref(results[i])));
        }

        for (std::thread& t : threads) {
            t.join();
        }

        int totalSum = 0;
        for (int sum : results) {
            totalSum += sum;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Время выполнения с " << numThreads << " потоками: " << duration.count() << " секунд" << std::endl;

        return totalSum;
    }

    // Вычисление суммы в одном потоке
    int calculateSingleThreadSum() {
        auto start = std::chrono::high_resolution_clock::now();

        int totalSum = 0;
        for (int num : array) {
            totalSum += num;
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        std::cout << "Время выполнения в одном потоке: " << duration.count() << " секунд" << std::endl;

        return totalSum;
    }
};

int main() {
    // Размер массива
    const size_t arraySize = 10000000; // 10 миллионов элементов
    std::vector<int> array(arraySize);

    // Заполнение массива случайными числами
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 100);
    for (size_t i = 0; i < arraySize; ++i) {
        array[i] = dist(gen);
    }

    // Количество ядер процессора
    const size_t numCores = std::thread::hardware_concurrency();
    std::cout << "Количество ядер процессора: " << numCores << std::endl;

    // Тестирование с разным количеством потоков
    std::vector<size_t> threadCounts = {1, 4, 8, 10};
    for (size_t threads : threadCounts) {
        ArraySumCalculator calculator(array, threads);
        int sum = calculator.calculateParallelSum();
        std::cout << "Сумма: " << sum << std::endl;
    }

    // Тестирование в одном потоке
    ArraySumCalculator singleThreadCalculator(array, 1);
    int singleThreadSum = singleThreadCalculator.calculateSingleThreadSum();
    std::cout << "Сумма в одном потоке: " << singleThreadSum << std::endl;

    return 0;
}

/*
*/
