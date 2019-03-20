#pragma once

class CUnit {
 public:
  //index of the unit to which this unit’s concentrate stream is connected 
  double conc_num[2];
  //index of the unit to which this unit’s concentrate stream is connected 
  double tails_num[2];
  //A Boolean that is changed to true if the unit has been seen 
  bool mark;
  double in[2];
  double oldin[2];
  CUnit();
  ~CUnit();
  void calc_stream();




	

  /*

    ...other member functions and variables of CUnit

  */
};

