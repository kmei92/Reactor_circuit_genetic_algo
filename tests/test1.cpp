#include <stdio.h>

#include "CUnit.h"
#include "CCircuit.h"

int main(int argc, char * argv[]){

        int valid[3] = {0, 1, 2};
        int invalid[3] = {0, 2, 2};

        if (Check_Validity(valid))
		printf("pass\n");
	else
		printf("fail\n");

/* if (Check_Validity(invalid))
		printf("fail\n");
	else
		printf("pass\n");
*/

}
