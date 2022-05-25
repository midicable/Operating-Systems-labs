#include "Employee.h"

Employee::Employee() {
    this->id = 0;
    this->hours = 0.;
}

Employee::Employee(int _id, const char* _name, double _hours) {
    this->id = _id;
    for (int i = 0; i < 10; i++)
        this->name[i] = _name[i];
    this->hours = _hours;
}