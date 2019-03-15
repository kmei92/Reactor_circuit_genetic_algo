#include <stdio.h>
#include <cmath>

#include "CUnit.h"
#include "CCircuit.h"
#include "Genetic_Algorithm.h"

int answer_vector[2*num_units+1] = {0, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 8, 9,
				10, 11, 10, 11, 10, 11, 10, 11}  ;

double Evaluate_Circuit(int *circuit_vector, double tolerance, int max_iterations) {

  double Performance = 0.0; 
  for (int i=0;i<num_units;i++) {
    //answer_vector is a predetermined answer vector (same size as circuit_vector)
    Performance += (20-std::abs(circuit_vector[i]-answer_vector[i]))*100.0; 
  }
  return Performance; 
}
