#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include "Employee.h"
#include "Windows.h"


int main(int argc, char* argv[])
{
    Employee                 emp;
    std::ifstream            in;
    std::ofstream            out;
    std::vector<Employee>    employees;
    std::string              binFileName             = argv[1];
    std::string              txtFileName             = argv[2];
    std::string              progressBarChunck       = "";
    int                      payPerHour              = std::stod(argv[3]);
    int                      progressBarChunckSize;

    in.open(binFileName, std::ios::binary);
    if (in.is_open()) {
        do {
            in.read((char*)&emp, sizeof(Employee));
            employees.push_back(emp);
        } while (!in.eof());
        employees.pop_back();
    }
    in.close();
    progressBarChunckSize = 100 / employees.size();
    for (int i = 0; i < progressBarChunckSize; i++)
        progressBarChunck += "#";


    out.open(txtFileName);
    if (out.is_open()) {
        out << "-------------------Report according to info.bin-------------------" << std::endl;
        std::cout << std::endl;
        out << "ID" << std::setw(20) << "NAME" << std::setw(20) << "SALARY" << std::endl;
        std::cout << "PROGRESS: ";
        for (Employee e : employees) {
            out << e.id << std::setw(21) << e.name << std::setw(20) << e.hours * payPerHour << std::endl;
            std::cout << progressBarChunck;
            Sleep(1000 / employees.size());
        }
    }
    out.close();
}