#include <iostream>
#include "CUnit.h"
#include "CCircuit.h"
//#include "CCircuit.cpp"


int main(int argc, char * argv[]){

        int valid[3] = {0, 1, 2};
        int invalid[3] = {0, 2, 2};
        int OneValid[7] = {0, 1, 2, 3, 0, 0, 4};
	std::cout << "Check_Validity({0,1,2}):\n";

        if (Check_Validity(valid))
	    std::cout  << "pass\n";
	else
	    std::cout << "fail\n";



/*      
	std::cout << "Check_Validity({0,2,2}):\n";
        if (Check_Validity(invalid))
            std::cout << "fail\n";
        else
            std::cout << "pass\n";
*/

}
