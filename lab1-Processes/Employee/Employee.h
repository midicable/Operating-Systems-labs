#include <iostream>

struct Employee {
    Employee();
    Employee(int, const char*, double);

    int             id;
    char            name[10];
    double          hours;
};
