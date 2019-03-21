#pragma once
#include <vector>

using namespace std;

class CUnit {
public:

	int conc_num, tails_num;
	//index of the unit to which this unit�s concentrate stream is connected
	double top[2];
	//index of the unit to which this unit�s concentrate stream is connected
	double bottom[2];
	//A Boolean that is changed to true if the unit has been seen
	bool mark;

	double feed[2];
	double old_feed[2];

	CUnit();
	~CUnit();
	void calc_stream();
	void store();
	void wipe_feed();

};