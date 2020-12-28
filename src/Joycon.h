#pragma once
#include "JoyShockLibrary.h"
#include "ofMain.h"
#include "ofApp.h"
#include "ofxOsc.h"
#include "sharedDefs.h"

#define RPRESSED 150
#define GPRESSED 150
#define BPRESSED 100//_n1
#define BASE_BUTTON_COLOR 25
#define MAX_GYRO_VALUE 4000
#define MAX_ACCEL_VALUE 8
#define GRAPH_WHITE ofColor(200, 200, 200, 200)
#define GRAPH_RED ofColor(255, 100, 100, 200)
#define GRAPH_GREEN ofColor(100, 255, 100, 200)
#define GRAPH_BLUE ofColor(100, 100, 255, 200)

class Joycon {
	private:
		int deviceId = -1;
		int intColor = 0xffffff;
		int controllerType; //left(1) or right(2) joycon identifier _n2
		IMU_STATE rawIMUData = IMU_STATE();
		MOTION_STATE cookedIMUData = MOTION_STATE();

		int positionOnList;
		ofTrueTypeFont font; 
		string oscNetAddress = DEFAULT_OSCSEND_ADDRESS;
		int oscSendPort = DEFAULT_OSCSEND_PORT;
		string joyconOscAddress = "";
		inputOSCTags inputOSCTags;

		bool useRawIMUData = true;
		bool useCookedIMUData = true;
		bool drawRawIMUData = true;
		bool drawCookedIMUData = true;
		bool useStickAsPolar = true;
		bool useStickAsDpad = true;
		float minStickStep = DEFAULT_MINSTICKSTEP;
		float minStickAsDpadDist = DEFAULT_MINSTICKASDPADDIST;

		ofPath joyconDrawing;
		ofPath upXButton;
		ofPath downBButton;
		ofPath leftYButton;
		ofPath rightAButton;
		ofPath minusPlusButton;
		ofPath zlzrButton;
		ofPath lrButton;
		ofPath printHomeButton;
		ofPath stickBase;
		ofPath stickButton;
		ofPath srButton;
		ofPath slButton;
		ofPath stickTargetArc;
		ofPath stickTargetLineUp;
		ofPath stickTargetLineDown;
		ofPath stickTargetLineLeft;
		ofPath stickTargetLineRight;
		float stickTargetRadius;
		float stickPointerRadius;
		float stickPointerCenterX;
		float stickPointerCenterY;
		float stickAsPolarX;
		float stickAsPolarY;
		float minWidthForStick;

		float dataGraphPosX = 0;
		float dataGraphPosY = 0;
		float dataGraphWidth = 0;
		vector<float> gyroXValues;
		vector<float> gyroYValues;
		vector<float> gyroZValues;
		vector<float> rawAccelXValues;
		vector<float> rawAccelYValues;
		vector<float> rawAccelZValues;
		vector<float> quatWValues;
		vector<float> quatIValues;
		vector<float> quatJValues;
		vector<float> quatKValues;
		vector<float> cookedAccelXValues;
		vector<float> cookedAccelYValues;
		vector<float> cookedAccelZValues;
		vector<float> gravityXValues;
		vector<float> gravityYValues;
		vector<float> gravityZValues;
		int IMUVectorsSize = DEFAULT_IMUVECTORSSIZE;
		int currentFirstPosGraphs = 0; //_n3

		void defineJoyconColor(int guiAlpha);
		void defineJoyconNameOnGUI(int devicesConnectedNumber);
		void oscSenderSetup();
		inputValues getEachInputValue(JOY_SHOCK_STATE newButtonsStickData);
		ofVec2f getStickAsPolarCoordinates(float stickX, float stickY);
		stickAsDpad setStickAsDpad(float stickX, float stickY);
		void draw2DGraph(float posX, float posY, float graphWidth, float graphHeight, vector<float> graphValuesI, vector<float> graphValuesJ, vector<float> graphValuesK, float maxYValue = 1, int numLayers = 4, vector<float> graphValuesW = vector<float>());
		ofColor getInputColor(int buttonValue, int baseRGB = BASE_BUTTON_COLOR);
		bool pointInsidePolylines(vector<ofPolyline> polylinesToCheck, int pointX, int pointY);
		string mouseClickAction(int mouseButton, bool &joyconButton, string joyconButtonOSCTag);

	public:
		inputValues currentInputValues;
		int celPosX = 0;
		int celPosY = 0;
		int celWidth = 0;
		int celHeight = 0;
		bool isVirtual = false; //_n4
		bool* clickedInputPointer = NULL;
		ofxToggle GUIToggle = true;
		ofxOscSender oscSender;
		ofColor joyconColor;
		string nameOnGUI;

		Joycon(int newDeviceId, int devicesConnectedNumber, int guiAlpha, ofTrueTypeFont loadedFont) {
			deviceId = newDeviceId;
			positionOnList = devicesConnectedNumber;
			font = loadedFont;
			if (deviceId == -1) {
				isVirtual = true;
				intColor = ofRandom(0x000000, 0xffffff);
				controllerType = ofRandom(1, 3);
			}
			else {
				intColor = JslGetControllerColour(deviceId);
				controllerType = JslGetControllerType(deviceId);
			}

			defineJoyconColor(guiAlpha);
			defineJoyconNameOnGUI(devicesConnectedNumber);
			oscSenderSetup();

			gyroXValues.resize(IMUVectorsSize, 0);
			gyroYValues.resize(IMUVectorsSize, 0);
			gyroZValues.resize(IMUVectorsSize, 0);
			rawAccelXValues.resize(IMUVectorsSize, 0);
			rawAccelYValues.resize(IMUVectorsSize, 0);
			rawAccelZValues.resize(IMUVectorsSize, 0);
			quatWValues.resize(IMUVectorsSize, 0);
			quatIValues.resize(IMUVectorsSize, 0);
			quatJValues.resize(IMUVectorsSize, 0);
			quatKValues.resize(IMUVectorsSize, 0);
			cookedAccelXValues.resize(IMUVectorsSize, 0);
			cookedAccelYValues.resize(IMUVectorsSize, 0);
			cookedAccelZValues.resize(IMUVectorsSize, 0);
			gravityXValues.resize(IMUVectorsSize, 0);
			gravityYValues.resize(IMUVectorsSize, 0);
			gravityZValues.resize(IMUVectorsSize, 0);
		};

		void updateData(JOY_SHOCK_STATE newButtonsStickData, IMU_STATE newRawIMUData);
		void sendNewInputsAsOSC(inputValues newInputValues);
		ofxOscMessage getInputOscMessage(string inputAddress, float inputValue);
		void updateGraphsValues();
		void updateDrawings(int newCelWidth, int newCelHeight, int newCelPosX, int newCelPosY);
		void drawJoycon();
		string checkMouseClick(int mouseClickX, int mouseClickY, int mouseButton);
};

/*
	n1- those 3 constants represent the color of the pressed buttons in rgb;
	n2- this is used with the constants defined on JoyShockLibrary. This library also identifies
		ProControllers(3) and DualShock(4), but that's not suported;
	n3- these float vectors are used to store the last 'IMUVectorsSize' values of IMU data, raw and cooked.
		This is used to draw the graphs with the data;
	n4- Virtual joycons are objects of type 'Joycon' that are not linked to connected divices. They are just
		visualizations used to test and configure preferences of each joycon input even when no 
		devices are connected;
*/