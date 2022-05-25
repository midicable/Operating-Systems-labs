#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include "Employee.h"

int main(int argc, char* argv[])
{
    Employee        emp;
    std::ofstream   out;
    std::string     binFileName     = argv[1];
    int             NumOfEntries    = std::stoi(argv[2]);

    out.open(binFileName, std::ios::app | std::ios::binary);
    if (out.is_open()) {
        for (int i = 0; i < NumOfEntries; i++) {
            std::cout << "Enter employee N." << i + 1 << " info: ";  std::cin >> emp.id >> emp.name >> emp.hours;
            out.write((char*)&emp, sizeof(Employee));
        }
    }
    out.close();
}