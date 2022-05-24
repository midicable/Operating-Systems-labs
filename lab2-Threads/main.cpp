#include <iostream>
#include "Windows.h"


int         size{ 0 };
int*        arr{ 0 };
int         minimum{ 0 };
int         maximum{ 0 };
int         minIdx{ 0 };
int         maxIdx{ 0 };
int         av{ 0 };
const int   THREADS_QUANTITY{ 2 };


DWORD WINAPI min_max(LPVOID) {
    minimum = arr[0];
    maximum = arr[0];
    for (int i = 0; i < size; i++) {
        if (arr[i] < minimum) {
            minimum = arr[i];
            minIdx = i;
            Sleep(12);
        }
        if (arr[i] > maximum) {
            maximum = arr[i];
            maxIdx = i;
            Sleep(12);
        }
    }
    std::cout << "MIN ELEMENT: " << minimum << std::endl;
    std::cout << "MAX ELEMENT: " << maximum << std::endl;

    return 0;
}

DWORD WINAPI average(LPVOID) {
    av = 0;
    for (int i = 0; i < size; i++) {
        av += arr[i];
        Sleep(12);
    }
    av /= size;
    std::cout << "AVERAGE: " << av << std::endl;

    return 0;
}


int main()
{
    std::cout << "Input array size: ";
    std::cin >> size;
    arr = new int[size];
    std::cout << "Input array elements: ";
    for (std::size_t i = 0; i < size; i++)
        std::cin >> arr[i];

    HANDLE hThreads[THREADS_QUANTITY];
    hThreads[0] = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    hThreads[1] = CreateThread(NULL, 0, average, NULL, 0, NULL);

    WaitForMultipleObjects(THREADS_QUANTITY, hThreads, TRUE, INFINITE);

    arr[minIdx] = av;
    arr[maxIdx] = av;

    std::cout << "ARRAY: ";
    for (std::size_t i = 0; i < size; i++)
        std::cout << arr[i] << ' ';
    std::cout << std::endl;

    for (std::size_t i = 0; i < THREADS_QUANTITY; i++)
        CloseHandle(hThreads[i]);

    return 0;
}

