#pragma once

struct note {
	int kind;
	long st, ed;
	bool preCheck = false;
	bool dead = false;
};