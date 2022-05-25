#include <iostream>
#include "Windows.h"


CRITICAL_SECTION    workingWithAnArray;
CRITICAL_SECTION    userInput;
HANDLE*             hStartThread            { nullptr };
HANDLE*             hThreads                { nullptr };
HANDLE*             hExitThread             { nullptr };
HANDLE*             hImpossibleToWork       { nullptr };
int*                arr                     { nullptr };
int                 size                    { 0 };
const int           DELAY                   { 5 };
const int           THREAD_FLAGS_QUANTITY   { 2 };


DWORD WINAPI marker(LPVOID _threadNum) {
    WaitForSingleObject(hStartThread[(int)_threadNum], INFINITE);

    int        threadNum            { (int)_threadNum };
    int        randomNum            { 0 };
    int        markedElements       { 0 };
    HANDLE     hThreadFlags[]       { hStartThread[threadNum], hExitThread[threadNum] };
    DWORD      dFlagNum             { 0 };


    while (true) {
        randomNum = rand() % size;
        if (arr[randomNum] == 0) {
            EnterCriticalSection(&workingWithAnArray);
            Sleep(DELAY);
            arr[randomNum] = threadNum + 1;
            markedElements++;
            Sleep(DELAY);
            LeaveCriticalSection(&workingWithAnArray);
        }
        else {
            EnterCriticalSection(&workingWithAnArray);
            std::cout << "thread number: " << threadNum + 1 << std::endl;
            std::cout << "number of marked elements: " << markedElements << std::endl;
            std::cout << "index impossible to mark: " << randomNum << std::endl;
            LeaveCriticalSection(&workingWithAnArray);
            SetEvent(hImpossibleToWork[threadNum]);
            ResetEvent(hStartThread[threadNum]);
            dFlagNum = WaitForMultipleObjects(THREAD_FLAGS_QUANTITY, hThreadFlags, FALSE, INFINITE);
            if (dFlagNum == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(&workingWithAnArray);
                for (std::size_t i = 0; i < size; i++)
                    if (arr[i] == threadNum + 1)
                        arr[i] = 0;
                LeaveCriticalSection(&workingWithAnArray);
                ExitThread(0);
            }
            else {
                ResetEvent(hImpossibleToWork[threadNum]);
                continue;
            }
        }
    }
}


int main()
{
    int        threadsQuantity      { 0 };
    int        threadToTerminate    { 0 };
    int        terminatedThreads    { 0 };
    bool*      isTerminated         { nullptr };

    InitializeCriticalSection(&workingWithAnArray);
    InitializeCriticalSection(&userInput);

    std::cout << "input array size: "; std::cin >> size;
    arr = new int[size] {};
    std::cout << "input threads quantity: "; std::cin >> threadsQuantity;

    hThreads = new HANDLE[threadsQuantity];                                       // массивы под n-ное количество потоков,
    hExitThread = new HANDLE[threadsQuantity];                                    // события о запросах на завершение i-того потока
    hImpossibleToWork = new HANDLE[threadsQuantity];                              // и события о невозможности i-того потока продолжить работу
    hStartThread = new HANDLE[threadsQuantity];
    isTerminated = new bool[threadsQuantity] {};

    for (std::size_t i = 0; i < threadsQuantity; i++) {
        hThreads[i] = CreateThread(NULL, 1, marker, (LPVOID)(i), NULL, NULL);     // создаем потоки
        hStartThread[i] = CreateEvent(NULL, TRUE, FALSE, NULL);                   // создаем события о начале работы i-того потока
        hExitThread[i] = CreateEvent(NULL, TRUE, FALSE, NULL);                    // создаем события запроса на завершение работы i-того потока
        hImpossibleToWork[i] = CreateEvent(NULL, TRUE, FALSE, NULL);              // создаем события невозможности работы i-того потока
    }

    while (terminatedThreads != threadsQuantity) {

        for (std::size_t i = 0; i < threadsQuantity; i++)
            if (!isTerminated[i]) {
                ResetEvent(hImpossibleToWork[i]);
                SetEvent(hStartThread[i]);
            }

        WaitForMultipleObjects(threadsQuantity, hImpossibleToWork, TRUE, INFINITE);

        EnterCriticalSection(&workingWithAnArray);
        std::cout << "ARRAY: ";
        for (std::size_t i = 0; i < size; i++)
            std::cout << arr[i] << ' ';
        std::cout << std::endl;
        LeaveCriticalSection(&workingWithAnArray);

        std::cout << "thread to terminate: "; std::cin >> threadToTerminate;
        if (isTerminated[threadToTerminate - 1])
            std::cout << "THIS THREAD HAS BEEN ALREADY TERMINATED" << std::endl;
        else {
            isTerminated[threadToTerminate - 1] = true;
            terminatedThreads++;
            SetEvent(hExitThread[threadToTerminate - 1]);
            WaitForSingleObject(hThreads[threadToTerminate - 1], INFINITE);
            CloseHandle(hThreads[threadToTerminate - 1]);
            CloseHandle(hExitThread[threadToTerminate - 1]);
            CloseHandle(hStartThread[threadToTerminate - 1]);
        }

        EnterCriticalSection(&workingWithAnArray);
        std::cout << "ARRAY: ";
        for (std::size_t i = 0; i < size; i++)
            std::cout << arr[i] << ' ';
        std::cout << std::endl;
        LeaveCriticalSection(&workingWithAnArray);
    }

    delete arr; arr = nullptr;
    delete isTerminated; isTerminated = nullptr;
    for (std::size_t i = 0; i < threadsQuantity; i++) 
        CloseHandle(hImpossibleToWork[i]);

    std::cout << "ALL THREADS HAS BEEN TERMINATED!" << std::endl;
}
