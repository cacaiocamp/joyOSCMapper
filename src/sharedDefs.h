#pragma once

#define DEFAULT_OSCSEND_ADDRESS "127.0.0.1"
#define DEFAULT_OSCSEND_PORT 11111
#define DEFAULT_OSCRECEIVE_PORT 22222
#define DEFAULT_OSCONLY_OSCADDRESS "/oscOnly"
#define DEFAULT_IMUVECTORSSIZE 67 //_n1
#define DEFAULT_MINSTICKSTEP 0.0035 //_n2
#define DEFAULT_MINSTICKASDPADDIST 0.2 //_n3

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

/*
	n1- change DEFAULT_IMUVECTORSSIZE if you want the graphs to show more than 1 second of each motion 
		capture axis. Note that increase this too much may cause fps drop,  depending also on the number 
		of joycons drawing;
	n2- change DEFAULT_MINSTICKSTEP if you are having any issues with stick precision. The stick values 
		reported are diferent almost every single report, 'minStickStep' represent the minimum stick 
		variation to trigger stick OSC messages and drawing;
	n3- 'minStickAsDpadDist' represent the minimum stick distance (as in polar coordinates) from the center 
		to trigger stickAsDpad values. For example, with the default value at 0.2, you need to have 
		'stickX >= 0.2 to trigger' 'stickAsDpad.right true' and 'stickX < 0.2' to 
		retrigger 'stickAsDpad.right false'.
*/