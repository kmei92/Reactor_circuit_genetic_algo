#include "CUnit.h"

CUnit::CUnit() {
	//conc_count = 0;
	//tail_count = 0;
	for (int i = 0; i < 2; i++)
	{
		top[i] = 0;
		bottom[i] = 0;
		feed[i] = 0;
		old_feed[i] = 0;
	}
	mark = false;
};

CUnit::~CUnit() {
};

void CUnit::calc_stream() {
	top[0] = feed[0] * 0.2;
	top[1] = feed[1] * 0.05;
	bottom[0] = feed[0] - top[0];
	bottom[1] = feed[1] - top[1];
};

void CUnit::store()
{
	for (int i = 0; i < 2; i++)
	{
		old_feed[i] = feed[i];
		feed[i] = 0;
	}
}

void CUnit::wipe_feed() {
	for (int i = 0; i < 2; i++)
		feed[i] = 0;
}