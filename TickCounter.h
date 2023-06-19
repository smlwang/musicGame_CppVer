#pragma once
#include <Windows.h>
class TickCounter {
	LARGE_INTEGER st, now, fry;
public:
	TickCounter() {
		QueryPerformanceCounter(&st);
		QueryPerformanceFrequency(&fry);
	}
	// return ms from a instance be created
	long long Now() {
		QueryPerformanceCounter(&now);
		return 10000 * (now.QuadPart - st.QuadPart) / fry.QuadPart / 10;
	}
};

