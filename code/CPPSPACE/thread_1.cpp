#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

std::mutex mtx;
std::condition_variable cv;
bool isOddTurn = true;
int count = 1;

void printOdd() {
    while (count <= 20) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return isOddTurn; });
        if (count <= 20) {
            std::cout << "Odd Thread: " << count << std::endl;
            count++;
            isOddTurn = false;
        }
        cv.notify_all();
    }
}

void printEven() {
    while (count <= 20) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return !isOddTurn; });
        if (count <= 20) {
            std::cout << "Even Thread: " << count << std::endl;
            count++;
            isOddTurn = true;
        }
        cv.notify_all();
    }
}

int main() {
    std::thread t1(printOdd);
    std::thread t2(printEven);

    t1.join();
    t2.join();

    return 0;
}