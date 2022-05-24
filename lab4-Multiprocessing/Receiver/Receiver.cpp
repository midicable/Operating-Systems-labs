#include <iostream>
#include <fstream>
#include <string>
#include "Windows.h"


int main() {
    STARTUPINFO         si                      { 0 };
    PROCESS_INFORMATION pi                      { 0 };				
    std::ifstream       binInputStream;
    std::string         binFileName;
    std::string         strCmdLineRequest;
    std::wstring        wstrCmdLineRequest;
    LPWSTR              lpwCmdLineRequest       { nullptr };
    HANDLE*             hProcesses              { nullptr };
    HANDLE*             hStartEvents            { nullptr };
    HANDLE              hCurrentProcess         { 0 };
    HANDLE              hReadAccessSemaphore    { 0 };
    HANDLE              hWriteAccessSemaphore   { 0 };
    DWORD               response                { 0 };
    const short         DELAY                   { 25'000 };
    const short         messageLength           { 20 };	
    char                message[20]             {   };
    int                 numberOfProcesees       { 0 };
    int                 numberOfEntries         { 0 };
    int                 bytesRead               { 0 };
    int                 option                  { 0 };


    std::cout << "Input .bin file name: "; std::cin >> binFileName;
    std::cout << "Input number of entries: "; std::cin >> numberOfEntries;
    std::cout << "Input number of Sender processes: "; std::cin >> numberOfProcesees;

    hCurrentProcess = CreateMutex(NULL, TRUE, L"Current Process Owner");
    hReadAccessSemaphore = CreateSemaphore(NULL, 0, numberOfEntries, L"Read Access Semaphore");
    hWriteAccessSemaphore = CreateSemaphore(NULL, numberOfEntries, numberOfEntries, L"Write Access Semaphore");

    hProcesses = new HANDLE[numberOfProcesees];
    hStartEvents = new HANDLE[numberOfProcesees];
    for (std::size_t i = 0; i < numberOfProcesees; i++)
        hStartEvents[i] = CreateEvent(NULL, TRUE, FALSE, (LPCWSTR)("START PROCESS" + i));

    for (std::size_t i = 0; i < numberOfProcesees; i++) {
        strCmdLineRequest = std::string("Sender.exe") + " " + binFileName + " " + std::to_string(i);
        wstrCmdLineRequest = std::wstring(strCmdLineRequest.begin(), strCmdLineRequest.end());
        lpwCmdLineRequest = &wstrCmdLineRequest[0];
        if (!CreateProcess(NULL, lpwCmdLineRequest, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
            std::cout << "Unable to create \"Sender.exe\" process" << std::endl;
        CloseHandle(pi.hThread);
        hProcesses[i] = pi.hProcess;
    }	

    WaitForMultipleObjects(numberOfProcesees, hStartEvents, TRUE, INFINITE);
    ReleaseMutex(hCurrentProcess);
    while (option != 2) {
        WaitForSingleObject(hCurrentProcess, INFINITE);                    // ждем, пока будет свободен мютекс для его захвата
        response = WaitForSingleObject(hReadAccessSemaphore, DELAY);       // ждем, пока в файле появятся записи
        if (response == WAIT_TIMEOUT) {
            std::cout << "[warning] Timeout: no response!" << std::endl;
            break;
        }
        std::cout << "Options: \n" << 
        "1. Read " + binFileName + '\n' <<
        "2. Exit programm \n";
        std::cin >> option;
        switch (option) {
        case 1:		
            option = 0;
            binInputStream.open(binFileName, std::ios::binary);             // открываем файл для чтения
            binInputStream.seekg(bytesRead);                                // ставим каретку на отступ равный количеству считанных байт
            if (binInputStream.tellg() != 0 || !binInputStream.eof()) {     // если файл пуст (достигнут конец файла), 
                binInputStream.read((char*)&message, sizeof(message));      // то читаем сообщение
                std::cout << "Message: ";
                for (std::size_t i = 0; i < messageLength; i++)
                    std::cout << message[i];
                std::cout << std::endl;
                binInputStream.close();
                bytesRead += messageLength;
                ReleaseSemaphore(hWriteAccessSemaphore, 1, NULL);           // грубо говоря объявляем, что появилось +1 место для записи в файл
            }	
            else                                                            // иначе больше нечего читать и процесс "отпускает" мьютекс текущего владельца процессом
                continue;
            break;
        case 2:
            std::cout << "Exiting process..." << std::endl;
            Sleep(500);
            break;
        }
    }

    CloseHandle(hCurrentProcess);
    CloseHandle(hReadAccessSemaphore);
    CloseHandle(hWriteAccessSemaphore);
    for (std::size_t i = 0; i < numberOfProcesees; i++)
        CloseHandle(hStartEvents[i]);
    delete[] hStartEvents; hStartEvents = nullptr;
    for (std::size_t i = 0; i < numberOfProcesees; i++)
        CloseHandle(hProcesses[i]);
    delete[] hProcesses; hProcesses = nullptr;

    return 0;		
}