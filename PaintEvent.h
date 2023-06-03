#pragma once
struct PaintEvent {
	int kind;
	long time = 0;
	int sizeLevel = 0;
	long lastTime = 0;
};