#pragma once
struct PaintEvent {
	int kind;
	long long time = 0;
	int sizeLevel = 0;
	long long lastTime = 0;
};