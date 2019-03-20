#include <vector>
#include <iostream>
#include <stdio.h>
#include "../includes/CUnit.h"
#include "../includes/CCircuit.h"


std::vector<CUnit> units(num_units_all);
int feed_num;

bool Check_Validity(int *circuit_vector)
{
    for(int i = 1; i < 2 * num_units + 1; i = i + 2){
        units[i/2].conc_num = circuit_vector[i];
        units[i/2].tails_num = circuit_vector[i+1];
        units[i/2].mark = false;
    }
    feed_num = circuit_vector[0];
    /*
    for (int i=0; i < num_units; i++)
        units[i].mark = false;
        */
   //std::cout <<"feed_num"<< feed_num << std::endl;
    mark_units(feed_num);
    // Criterion 1
    for (int i = 0; i < num_units; i++) {
        if (!units[i].mark) {
            std::cout <<"id: " << i <<" Criterion 1 fail!"<< std::endl;
            return false;
        }
    }

    for (int i = 1; i < num_units * 2 + 1; i = i + 2){

        // Criterion 3 and Criterion 4
        if (circuit_vector[i] == i/2 ||circuit_vector[i+1] == i/2 || circuit_vector[i] == circuit_vector[i+1]){
            std::cout <<" Criterion 3 and Criterion 4 fail!"<< std::endl;
            return false;
        }

        // Criterion 2
        mark_units(i/2);
        if(!units[num_units_all - 1].mark || !units[num_units_all - 2].mark){
            std::cout << "i:"<< i <<" Criterion 2 fail!"<< std::endl;
            return false;
        }

        // reset the mark for next check
        for(int j = 0; j < num_units_all ; j++){
            units[j].mark = false;
        }

    }

    return true;
}

void mark_units(int unit_num) {

    if (units[unit_num].mark) return;

    units[unit_num].mark = true;
    //If we have seen this unit already exit
    //Mark that we have now seen the unit
    //If conc_num does not point at a circuit outlet recursively call the function

    if (units[unit_num].conc_num<num_units) {
        mark_units(units[unit_num].conc_num);
    } else {
        // ...Potentially do something to indicate that you have seen an exit
        //std::cout << " See the top! " << std::endl;
        units[units[unit_num].conc_num].mark = true;
    }
    //If tails_num does not point at a circuit outlet recursively call the function

    if (units[unit_num].tails_num<num_units) {
        mark_units(units[unit_num].tails_num);
    } else {
        // ...Potentially do something to indicate that you have seen an exit
        //std::cout << " See the bottom!" << std::endl;
        units[units[unit_num].tails_num].mark = true;
    }

}


