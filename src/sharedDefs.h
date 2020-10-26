#pragma once

#define MAX_GYRO_VALUE 4000
#define MAX_ACCEL_VALUE 8

typedef struct inputValues
{
	bool upX = 0;
	bool downB = 0;
	bool leftY = 0;
	bool rightA = 0;
	bool minusPlus = 0;
	bool zlzr = 0;
	bool lr = 0;
	bool printHome = 0;
	bool stickClick = 0;
	bool sr = 0;
	bool sl = 0;
	float stickX = 0;
	float stickY = 0;
} inputValues;