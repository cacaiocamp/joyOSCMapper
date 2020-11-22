#pragma once

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