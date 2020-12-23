#pragma once

#define BORDER 5

struct stickAsDpad {
	bool up = 0;
	bool down = 0;
	bool left = 0;
	bool right = 0;
};

struct inputValues
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
	float stickDistance = 0;
	float stickAzimuth = 0;
	stickAsDpad stickAsDpad;
};

struct inputOSCTags
{
	string upX = "/upX";
	string downB = "/downB";
	string leftY = "/leftY";
	string rightA = "/rightA";
	string minusPlus = "/minusPlus";
	string zlzr = "/zlzr";
	string lr = "/lr";
	string printHome = "/printHome";
	string stickClick = "/stickClick";
	string sr = "/sr";
	string sl = "/sl";

	string stickX = "/stickX";
	string stickY = "/stickY";
	string stickDistance = "/stickDistance";
	string stickAzimuth = "/stickAzimuth";
	string stickAsDpadUp = "/stickAsDpadUp";
	string stickAsDpadDown = "/stickAsDpadDown";
	string stickAsDpadLeft = "/stickAsDpadLeft";
	string stickAsDpadRight = "/stickAsDpadRight";

	string gyroX = "/gyroX";
	string gyroY = "/gyroY";
	string gyroZ = "/gyroZ";
	string raclX = "/raclX";
	string raclY = "/raclY";
	string raclZ = "/raclZ";

	string quatW = "/quatW";
	string quatX = "/quatX";
	string quatY = "/quatY";
	string quatZ = "/quatZ";
	string caclX = "/caclX";
	string caclY = "/caclY";
	string caclZ = "/caclZ";
	string gravX = "/gravX";
	string gravY = "/gravY";
	string gravZ = "/gravZ";
};