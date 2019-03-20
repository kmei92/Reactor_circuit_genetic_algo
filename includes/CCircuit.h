#ifndef CCIRCUIT_H
#define CCIRCUIT_H

#include<vector>
const int num_units = 10;



const int num_units_all = num_units + 2;

bool Check_Validity(int *circuit_vector);

void mark_units(int unit_num);

#endif //CCIRCUIT_H