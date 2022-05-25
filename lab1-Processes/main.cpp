#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <Windows.h>
#include <conio.h>
#include "Employee.h"


void displayBinFile(std::string fileName) {
    Employee                 emp;
    std::vector<Employee>    employees;
    std::ifstream            in;

    in.open(fileName, std::ios::binary);
    if (in.is_open()) {
        do {
            in.read((char*)&emp, sizeof(Employee));
            employees.push_back(emp);
        } while (!in.eof());
        employees.pop_back();
    }
    for (Employee e : employees)
        std::cout << "{id: " << e.id << 
                    "; name: " << e.name << 
                    "; hours:" << e.hours << "}" << std::endl;
}



int main()
{
    STARTUPINFO            si = { 0 };                // задает параметры окна, созданного новым процессом
    PROCESS_INFORMATION    pi = { 0 };                // содержит информацию о созданом процессе и его первичном потоке
    DWORD                  excode;                    // код при выходе из процесса
    std::string            binFileName;               // имя бинарного файла, куда будут сериализоваться рабочие
    std::string            txtFileName;               // имя файла, где содержится отчет по сотрудникам
    std::string            numOfEntries;              // число записей в бинарник
    std::string            payPerHour;                // оплата за час


    std::cout << "Main process has been started" << std::endl;
    std::cout << "Input filename: "; std::cin >> binFileName;
    std::cout << "Input number of entiries: "; std::cin >> numOfEntries;


    std::string            cmdCreatorRequest = std::string("Creator.exe") + " " + binFileName + " " + numOfEntries;
    std::wstring           creatorCmdLine(cmdCreatorRequest.begin(), cmdCreatorRequest.end());
    LPWSTR                 lpwCreatorCmdLine = &creatorCmdLine[0];
    BOOL creatorHasCreated = CreateProcess(NULL,
                                    lpwCreatorCmdLine,
                                    NULL, 
                                    NULL, 
                                    TRUE,
                                    CREATE_NEW_CONSOLE, 
                                    NULL,
                                    NULL,
                                    &si,
                                    &pi);
    if (!creatorHasCreated)
        std::cout << "Unable to create creator proccess" << std::endl;
    WaitForSingleObject(pi.hProcess, INFINITE);


    displayBinFile(binFileName);


    std::cout << "Enter report filename: "; std::cin >> txtFileName;
    std::cout << "Enter pay per hour: "; std::cin >> payPerHour;


    std::string        cmdReporterRequest = std::string("Reporter.exe") + " " + binFileName + " " + txtFileName + " " + payPerHour;
    std::wstring       reporterCmdLine(cmdReporterRequest.begin(), cmdReporterRequest.end());
    LPWSTR             lpwReporterCmdLine = &reporterCmdLine[0];
    BOOL reporterHasCreated = CreateProcess(NULL,
                                    lpwReporterCmdLine,
                                    NULL, 
                                    NULL, 
                                    TRUE,
                                    CREATE_NEW_CONSOLE, 
                                    NULL,
                                    NULL,
                                    &si,
                                    &pi);
    if (!reporterHasCreated)
        std::cout << "Unable to create reporter process" << std::endl;
    WaitForSingleObject(pi.hProcess, INFINITE);
    std::cout << "Reporter process has been terminated" << std::endl;



    _getch();
    GetExitCodeProcess(pi.hProcess, &excode);

    if (excode != STILL_ACTIVE) {
        std::cout << "Done" << std::endl;
    }
    else {
        std::cout << "Process still running" << std::endl;
    }

    std::cout << "Main process has been terminated" << std::endl;
    _getch();

}