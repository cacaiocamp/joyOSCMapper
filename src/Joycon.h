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
	public:
		int deviceId = -1;
		int intColor = 0xffffff;
		int controllerType; //left(1) or right(2) joycon identifier _n2
		IMU_STATE rawIMUData = IMU_STATE();
		MOTION_STATE cookedIMUData = MOTION_STATE();

		bool isVirtual = false; //_n3
		int positionOnList;
		ofColor joyconColor;
		ofxToggle GUIToggle;
		string nameOnGUI;
		ofTrueTypeFont font; 
		ofxOscSender oscSender;
		string oscNetAddress = "127.0.0.1";
		int oscSendPort = 11111;
		string joyconOscAddress = "";
		inputOSCTags inputOSCTags;
		inputValues currentInputValues;
		bool* clickedInputPointer = NULL;

		int celPosX = 0;
		int celPosY = 0; 
		int celWidth = 0;
		int celHeight = 0;
		bool useRawIMUData = true;
		bool useCookedIMUData = true;
		bool drawRawIMUData = true;
		bool drawCookedIMUData = true;
		bool useStickAsPolar = true;
		bool useStickAsDpad = true;
		float minStickStep = 0.0025;
		float stickAsDpadProp = 0.2;

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
		int IMUVectorsSize = 67;
		int currentFirstPosGraphs = 0; //_n4

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

		void defineJoyconColor(int guiAlpha) {
			int colorR = (intColor >> 16) & 0x0ff;
			int colorG = (intColor >> 8) & 0x0ff;
			int colorB = (intColor) & 0x0ff;

			joyconColor = ofColor(colorR, colorG, colorB, guiAlpha);
		}

		 void defineJoyconNameOnGUI(int devicesConnectedNumber) {
			 string prefix = "joycon";
			 if (isVirtual)
				 prefix = "vrtjoy";

			switch (controllerType) {
				case JS_TYPE_JOYCON_LEFT:
					nameOnGUI = prefix + "(L)" + ofToString(devicesConnectedNumber);
					break;
				case JS_TYPE_JOYCON_RIGHT:
					nameOnGUI = prefix + "(R)" + ofToString(devicesConnectedNumber);
					break;
				default:
					nameOnGUI = prefix + "(?)" + ofToString(devicesConnectedNumber);
					break;
			}

			joyconOscAddress = "/" + prefix + ofToString(devicesConnectedNumber);
		}

		void oscSenderSetup() {
			bool success = oscSender.setup(oscNetAddress, oscSendPort);
			if (!success)
				cout << "Could not connect to the net address " << oscNetAddress << ", with port " << oscSendPort << ";" << endl;
		}

		void updateData(JOY_SHOCK_STATE newButtonsStickData, IMU_STATE newRawIMUData){
			inputValues newInputValues = getEachInputValue(newButtonsStickData);
			rawIMUData = newRawIMUData;
			cookedIMUData = JslGetMotionState(deviceId);
			sendNewInputsAsOSC(newInputValues);
			currentInputValues = newInputValues;
		}

		inputValues getEachInputValue(JOY_SHOCK_STATE newButtonsStickData) {//_n5
			int buttonsData = newButtonsStickData.buttons;
			inputValues newInputValues;

			if (controllerType == JS_TYPE_JOYCON_LEFT) {
				newInputValues.upX = (buttonsData & JSMASK_UP) == JSMASK_UP;
				newInputValues.downB = (buttonsData & JSMASK_DOWN) == JSMASK_DOWN;
				newInputValues.leftY = (buttonsData & JSMASK_LEFT) == JSMASK_LEFT;
				newInputValues.rightA = (buttonsData & JSMASK_RIGHT) == JSMASK_RIGHT;
				newInputValues.minusPlus = (buttonsData & JSMASK_MINUS) == JSMASK_MINUS;
				newInputValues.stickClick = (buttonsData & JSMASK_LCLICK) == JSMASK_LCLICK;
				newInputValues.lr = (buttonsData & JSMASK_L) == JSMASK_L;
				newInputValues.zlzr = (buttonsData & JSMASK_ZL) == JSMASK_ZL;
				newInputValues.printHome = (buttonsData & JSMASK_CAPTURE) == JSMASK_CAPTURE;
				newInputValues.stickX = newButtonsStickData.stickLX;
				newInputValues.stickY = newButtonsStickData.stickLY;
			}
			else {
				newInputValues.upX = (buttonsData & JSMASK_N) == JSMASK_N;
				newInputValues.downB = (buttonsData & JSMASK_S) == JSMASK_S;
				newInputValues.leftY = (buttonsData & JSMASK_W) == JSMASK_W;
				newInputValues.rightA = (buttonsData & JSMASK_E) == JSMASK_E;
				newInputValues.minusPlus = (buttonsData & JSMASK_PLUS) == JSMASK_PLUS;
				newInputValues.stickClick = (buttonsData & JSMASK_RCLICK) == JSMASK_RCLICK;
				newInputValues.lr = (buttonsData & JSMASK_R) == JSMASK_R;
				newInputValues.zlzr = (buttonsData & JSMASK_ZR) == JSMASK_ZR;
				newInputValues.printHome = (buttonsData & JSMASK_HOME) == JSMASK_HOME;
				newInputValues.stickX = newButtonsStickData.stickRX;
				newInputValues.stickY = newButtonsStickData.stickRY; 
				
			}
			newInputValues.sl = (buttonsData & JSMASK_SL) == JSMASK_SL;
			newInputValues.sr = (buttonsData & JSMASK_SR) == JSMASK_SR;

			if (useStickAsPolar) {
				ofVec2f newPolarCoords = getStickAsPolarCoordinates(newInputValues.stickX, newInputValues.stickY);
				newInputValues.stickDistance = newPolarCoords.x;
				newInputValues.stickAzimuth = newPolarCoords.y;
			}
			if (useStickAsDpad) {
				stickAsDpad newStickAsDpad = setStickAsDpad(newInputValues.stickX, newInputValues.stickY);
				newInputValues.stickAsDpad = newStickAsDpad;
			}

			return newInputValues;
		}

		void sendNewInputsAsOSC(inputValues newInputValues) {
			if (!isVirtual) {
				if (newInputValues.upX != currentInputValues.upX)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.upX, newInputValues.upX));
				if (newInputValues.downB != currentInputValues.downB)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.downB, newInputValues.downB));
				if (newInputValues.leftY != currentInputValues.leftY)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.leftY, newInputValues.leftY));
				if (newInputValues.rightA != currentInputValues.rightA)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.rightA, newInputValues.rightA));
				if (newInputValues.minusPlus != currentInputValues.minusPlus)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.minusPlus, newInputValues.minusPlus));
				if (newInputValues.stickClick != currentInputValues.stickClick)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickClick, newInputValues.stickClick));
				if (newInputValues.lr != currentInputValues.lr)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.lr, newInputValues.lr));
				if (newInputValues.zlzr != currentInputValues.zlzr)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.zlzr, newInputValues.zlzr));
				if (newInputValues.printHome != currentInputValues.printHome)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.printHome, newInputValues.printHome));
				if (newInputValues.sl != currentInputValues.sl)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.sl, newInputValues.sl));
				if (newInputValues.sr != currentInputValues.sr)
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.sr, newInputValues.sr));
				if (abs(newInputValues.stickX - currentInputValues.stickX) >= minStickStep || abs(newInputValues.stickY - currentInputValues.stickY) >= minStickStep) {
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickX, newInputValues.stickX));
					oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickY, newInputValues.stickY));

					if (useStickAsPolar) {
						oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickDistance, currentInputValues.stickDistance));
						oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAzimuth, currentInputValues.stickAzimuth));
					}

					if (useStickAsDpad) {
						if(newInputValues.stickAsDpad.up != currentInputValues.stickAsDpad.up)
							oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadUp, newInputValues.stickAsDpad.up));
						if (newInputValues.stickAsDpad.down != currentInputValues.stickAsDpad.down)
							oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadDown, newInputValues.stickAsDpad.down));
						if (newInputValues.stickAsDpad.left != currentInputValues.stickAsDpad.left)
							oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadLeft, newInputValues.stickAsDpad.left));
						if (newInputValues.stickAsDpad.right != currentInputValues.stickAsDpad.right)
							oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadRight, newInputValues.stickAsDpad.right));
					}
				}
			}

			if (useRawIMUData) {
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.gyroX, rawIMUData.gyroX));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.gyroY, rawIMUData.gyroY));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.gyroZ, rawIMUData.gyroZ));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.raclX, rawIMUData.accelX));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.raclY, rawIMUData.accelY));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.raclZ, rawIMUData.accelZ));
			}
			if (useCookedIMUData) {
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.quatW, cookedIMUData.quatW));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.quatX, cookedIMUData.quatX));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.quatY, cookedIMUData.quatY));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.quatZ, cookedIMUData.quatZ));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.caclX, cookedIMUData.accelX));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.caclY, cookedIMUData.accelY));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.caclZ, cookedIMUData.accelZ));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.gravX, cookedIMUData.gravX));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.gravY, cookedIMUData.gravY));
				oscSender.sendMessage(getInputOscMessage(inputOSCTags.gravZ, cookedIMUData.gravZ));
			}
		}

		ofxOscMessage getInputOscMessage(string inputAddress, float inputValue) {
			ofxOscMessage inputMessage;
			inputMessage.setAddress(joyconOscAddress + inputAddress);
			inputMessage.addFloatArg(inputValue);
			return inputMessage;
		};

		ofVec2f getStickAsPolarCoordinates(float stickX, float stickY) {
			return ofVec2f(sqrt(pow(stickX, 2) + pow(stickY, 2)), atan2(stickY, stickX));
		}

		stickAsDpad setStickAsDpad(float stickX, float stickY) {
			stickAsDpad newStickAsDpad;
			if (abs(stickX) > stickAsDpadProp) {
				if (stickX < 0) {
					newStickAsDpad.left = false;
					newStickAsDpad.right = true;
				}
				else {
					newStickAsDpad.left = true;
					newStickAsDpad.right = false;
				}
			}
			else {
				newStickAsDpad.left = false;
				newStickAsDpad.right = false;
			}
			if (abs(stickY) > stickAsDpadProp) {
				if (stickY > 0) {
					newStickAsDpad.up = true;
					newStickAsDpad.down = false;
				}
				else {
					newStickAsDpad.up = false;
					newStickAsDpad.down = true;
				}
			}
			else {
				newStickAsDpad.up = false;
				newStickAsDpad.down = false;
			}
			return newStickAsDpad;
		}

		void updateGraphsValues() {
			if (isVirtual) {
				rawIMUData.gyroX = ofRandom(-MAX_GYRO_VALUE, MAX_GYRO_VALUE);
				rawIMUData.gyroY = ofRandom(-MAX_GYRO_VALUE, MAX_GYRO_VALUE);
				rawIMUData.gyroZ = ofRandom(-MAX_GYRO_VALUE, MAX_GYRO_VALUE);
				rawIMUData.accelX = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE);
				rawIMUData.accelY = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE);
				rawIMUData.accelZ = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE);
				gyroXValues[currentFirstPosGraphs] = rawIMUData.gyroX;
				gyroYValues[currentFirstPosGraphs] = rawIMUData.gyroY;
				gyroZValues[currentFirstPosGraphs] = rawIMUData.gyroZ;
				rawAccelXValues[currentFirstPosGraphs] = rawIMUData.accelX;
				rawAccelYValues[currentFirstPosGraphs] = rawIMUData.accelY;
				rawAccelZValues[currentFirstPosGraphs] = rawIMUData.accelZ;

				cookedIMUData.quatW = ofRandom(-1, 1);
				cookedIMUData.quatX = ofRandom(-1, 1);
				cookedIMUData.quatY = ofRandom(-1, 1);
				cookedIMUData.quatZ = ofRandom(-1, 1);
				cookedIMUData.accelX = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE);
				cookedIMUData.accelY = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE);
				cookedIMUData.accelZ = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE);
				cookedIMUData.gravX = ofRandom(-1, 1);
				cookedIMUData.gravY = ofRandom(-1, 1);
				cookedIMUData.gravZ = ofRandom(-1, 1);
				quatWValues[currentFirstPosGraphs] = cookedIMUData.quatW;
				quatIValues[currentFirstPosGraphs] = cookedIMUData.quatX;
				quatJValues[currentFirstPosGraphs] = cookedIMUData.quatY;
				quatKValues[currentFirstPosGraphs] = cookedIMUData.quatZ;
				cookedAccelXValues[currentFirstPosGraphs] = cookedIMUData.accelX;
				cookedAccelYValues[currentFirstPosGraphs] = cookedIMUData.accelY;
				cookedAccelZValues[currentFirstPosGraphs] = cookedIMUData.accelZ;
				gravityXValues[currentFirstPosGraphs] = cookedIMUData.gravX;
				gravityYValues[currentFirstPosGraphs] = cookedIMUData.gravY;
				gravityZValues[currentFirstPosGraphs] = cookedIMUData.gravZ;
			}
			else {
				if (drawRawIMUData) {
					gyroXValues[currentFirstPosGraphs] = rawIMUData.gyroX;
					gyroYValues[currentFirstPosGraphs] = rawIMUData.gyroY;
					gyroZValues[currentFirstPosGraphs] = rawIMUData.gyroZ;
					rawAccelXValues[currentFirstPosGraphs] = rawIMUData.accelX;
					rawAccelYValues[currentFirstPosGraphs] = rawIMUData.accelY;
					rawAccelZValues[currentFirstPosGraphs] = rawIMUData.accelZ;
				}
				if (drawCookedIMUData) {
					quatWValues[currentFirstPosGraphs] = cookedIMUData.quatW;
					quatIValues[currentFirstPosGraphs] = cookedIMUData.quatX;
					quatJValues[currentFirstPosGraphs] = cookedIMUData.quatY;
					quatKValues[currentFirstPosGraphs] = cookedIMUData.quatZ;
					cookedAccelXValues[currentFirstPosGraphs] = cookedIMUData.accelX;
					cookedAccelYValues[currentFirstPosGraphs] = cookedIMUData.accelY;
					cookedAccelZValues[currentFirstPosGraphs] = cookedIMUData.accelZ;
					gravityXValues[currentFirstPosGraphs] = cookedIMUData.gravX;
					gravityYValues[currentFirstPosGraphs] = cookedIMUData.gravY;
					gravityZValues[currentFirstPosGraphs] = cookedIMUData.gravZ;
				}
			}
			currentFirstPosGraphs++;
			if (currentFirstPosGraphs > IMUVectorsSize - 1) {
				currentFirstPosGraphs = 0;
			}
		};

		void updateDrawings(int newCelWidth, int newCelHeight, int newCelPosX, int newCelPosY) {
			celWidth = newCelWidth;
			celHeight = newCelHeight;
			celPosX = newCelPosX;
			celPosY = newCelPosY;

			float heightDivision = celHeight / 9;
			float widthDivision = heightDivision * 2.25;
			float fourButtonsRadius = heightDivision / 4;
			float stickRadius = heightDivision / 2.5;
			float horizontalBorder = (BORDER * 2) + (fourButtonsRadius / 3); 
			dataGraphPosY = (heightDivision / 2) + BORDER;
			minWidthForStick = 2 * widthDivision;

			float joyconRightEdgeX = 0;
			float joyconLeftEdgeX = 0;
			float joyconRightEdgeUpperY = 0;
			float joyconLeftEdgeUpperY = 0;
			float joyconRightEdgeLowerY = 0;
			float joyconLeftEdgeLowerY = 0;
			float joyconCurvesCenterX = 0;
			float upperCurveAngleBegin = 0;
			float lowerCurveAngleBegin = 0;
			float fourButtonsCenterY = 0;
			float stickCenterY = 0;
			float stickVisualizationCenterX = 0;

			joyconDrawing.clear();
			upXButton.clear();
			downBButton.clear();
			leftYButton.clear();
			rightAButton.clear();
			minusPlusButton.clear();
			zlzrButton.clear();
			lrButton.clear();
			printHomeButton.clear();
			stickBase.clear();
			stickButton.clear();
			srButton.clear();
			slButton.clear();
			stickTargetArc.clear();
			stickTargetLineUp.clear();
			stickTargetLineDown.clear();
			stickTargetLineLeft.clear();
			stickTargetLineRight.clear();

			if (controllerType == JS_TYPE_JOYCON_LEFT) { //_n6
				joyconRightEdgeX = celPosX + celWidth - horizontalBorder;
				joyconLeftEdgeX = celPosX + celWidth - (widthDivision + horizontalBorder);
				joyconCurvesCenterX = joyconRightEdgeX;
				joyconRightEdgeUpperY = celPosY + heightDivision;
				joyconLeftEdgeUpperY = celPosY + (heightDivision * 2);
				joyconRightEdgeLowerY = celPosY + (celHeight - heightDivision);
				joyconLeftEdgeLowerY = celPosY + (celHeight - (heightDivision * 2));
				upperCurveAngleBegin = 180;
				lowerCurveAngleBegin = 90;
				fourButtonsCenterY = celPosY + (celHeight / 2) + (heightDivision / 2);
				stickCenterY = celPosY + (celHeight / 2) - (heightDivision*1.5);

				minusPlusButton.rectangle(joyconRightEdgeX - (widthDivision / 4), joyconRightEdgeUpperY + (heightDivision / 2), (1.25*fourButtonsRadius), 1);
				printHomeButton.rectangle(joyconRightEdgeX - (widthDivision / 2.5), joyconLeftEdgeLowerY - (heightDivision / 1.5), 1.5*fourButtonsRadius, 1.5 * fourButtonsRadius);
				srButton.rectangle(joyconRightEdgeX + 1, fourButtonsCenterY + (2 * fourButtonsRadius), (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				slButton.rectangle(joyconRightEdgeX + 1, stickCenterY - fourButtonsRadius, (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				lrButton.arc(joyconCurvesCenterX - (fourButtonsRadius / 1.5), celPosY + (heightDivision * 2) - (fourButtonsRadius / 1.5), widthDivision - (fourButtonsRadius / 1.5), heightDivision - (fourButtonsRadius / 2), upperCurveAngleBegin, upperCurveAngleBegin + 90);
				zlzrButton.arc(joyconCurvesCenterX - (widthDivision / 2.325), celPosY + heightDivision + (heightDivision / 1.5), widthDivision / 1.75, heightDivision / 1.5, upperCurveAngleBegin, upperCurveAngleBegin + 90);

				stickVisualizationCenterX = joyconLeftEdgeX - horizontalBorder - (stickRadius * 1.75);
				dataGraphPosX = celPosX + (2 * BORDER);
				dataGraphWidth = ((stickVisualizationCenterX - BORDER - (stickRadius * 1.75) - horizontalBorder - celPosX) / 2) - BORDER;
			}
			else {
				joyconLeftEdgeX = celPosX + horizontalBorder;
				joyconRightEdgeX = celPosX + widthDivision + horizontalBorder;
				joyconCurvesCenterX = joyconLeftEdgeX;
				joyconRightEdgeUpperY = celPosY + (heightDivision * 2);
				joyconLeftEdgeUpperY = celPosY + heightDivision;
				joyconRightEdgeLowerY = celPosY + (celHeight - (heightDivision * 2));
				joyconLeftEdgeLowerY = celPosY + (celHeight - heightDivision);
				upperCurveAngleBegin = 270;
				lowerCurveAngleBegin = 0;
				fourButtonsCenterY = celPosY + (celHeight / 2) - (heightDivision*1.5);
				stickCenterY = celPosY + (celHeight / 2) + (heightDivision / 2);

				minusPlusButton.rectangle(joyconLeftEdgeX + (widthDivision / 4), joyconLeftEdgeUpperY + (heightDivision / 2) - 0.5, -1*(1.25*fourButtonsRadius), 1);
				minusPlusButton.rectangle(joyconLeftEdgeX + (widthDivision / 4) - (0.625*fourButtonsRadius) - 0.5, joyconLeftEdgeUpperY + (heightDivision / 2) - (0.625*fourButtonsRadius), 1, (1.25*fourButtonsRadius));
				printHomeButton.circle(joyconLeftEdgeX + (widthDivision / 2.5), joyconRightEdgeLowerY - (heightDivision / 2), fourButtonsRadius);
				srButton.rectangle(joyconLeftEdgeX - (fourButtonsRadius / 3) - 1, fourButtonsCenterY - fourButtonsRadius, (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				slButton.rectangle(joyconLeftEdgeX - (fourButtonsRadius / 3) - 1, stickCenterY + (2 * fourButtonsRadius), (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				lrButton.arc(joyconCurvesCenterX + (fourButtonsRadius / 1.5), celPosY + (heightDivision * 2) - (fourButtonsRadius / 1.5), widthDivision - (fourButtonsRadius / 1.5), heightDivision - (fourButtonsRadius / 2), upperCurveAngleBegin, upperCurveAngleBegin + 90);
				zlzrButton.arc(joyconCurvesCenterX + (widthDivision / 2.325), celPosY + heightDivision + (heightDivision / 1.5), widthDivision / 1.75, heightDivision / 1.5, upperCurveAngleBegin, upperCurveAngleBegin + 90);

				stickVisualizationCenterX = joyconRightEdgeX + horizontalBorder + (stickRadius * 1.75);
				dataGraphPosX = stickVisualizationCenterX + (stickRadius * 1.75) + horizontalBorder;
				dataGraphWidth = ((celWidth - (stickVisualizationCenterX + (stickRadius * 1.75) + BORDER + horizontalBorder - celPosX)) / 2) - BORDER;
			}

			zlzrButton.setStrokeColor(ofColor(150, 150, 150, 200));
			zlzrButton.setStrokeWidth(1);
			zlzrButton.setFillColor(getInputColor(currentInputValues.zlzr));
			zlzrButton.setFilled(true);
			zlzrButton.close();

			lrButton.setStrokeColor(ofColor(150, 150, 150, 200));
			lrButton.setStrokeWidth(1);
			lrButton.setFillColor(getInputColor(currentInputValues.lr));
			lrButton.setFilled(true);
			lrButton.close();

			joyconDrawing.arc(joyconCurvesCenterX, celPosY + (heightDivision * 2), widthDivision, heightDivision, upperCurveAngleBegin, upperCurveAngleBegin + 90);
			joyconDrawing.arc(joyconCurvesCenterX, celPosY + (celHeight - (heightDivision * 2)), widthDivision, heightDivision, lowerCurveAngleBegin, lowerCurveAngleBegin + 90);
			joyconDrawing.moveTo(joyconLeftEdgeX, joyconLeftEdgeUpperY);
			joyconDrawing.lineTo(joyconLeftEdgeX, joyconLeftEdgeLowerY);
			joyconDrawing.moveTo(joyconRightEdgeX, joyconRightEdgeUpperY);
			joyconDrawing.lineTo(joyconRightEdgeX, joyconRightEdgeLowerY);
			joyconDrawing.setStrokeColor(ofColor(255, 255, 255, 255));
			joyconDrawing.setStrokeWidth(1);
			joyconDrawing.setFillColor(joyconColor);
			joyconDrawing.setFilled(true);
			joyconDrawing.close();

			upXButton.arc(joyconRightEdgeX - (widthDivision / 2), fourButtonsCenterY - (heightDivision / 2), fourButtonsRadius, fourButtonsRadius, 0, 360);
			downBButton.arc(joyconRightEdgeX - (widthDivision / 2), fourButtonsCenterY + (heightDivision / 2), fourButtonsRadius, fourButtonsRadius, 0, 360);
			leftYButton.arc(joyconRightEdgeX - (widthDivision / 2) - (fourButtonsRadius * 2), fourButtonsCenterY, fourButtonsRadius, fourButtonsRadius, 0, 360);
			rightAButton.arc(joyconRightEdgeX - (widthDivision / 2) + (fourButtonsRadius * 2), fourButtonsCenterY, fourButtonsRadius, fourButtonsRadius, 0, 360);

			stickBase.arc(joyconRightEdgeX - (widthDivision / 2), stickCenterY, stickRadius + (stickRadius / 3), stickRadius + (stickRadius / 3), 0, 360);
			stickButton.setFillColor(getInputColor(currentInputValues.stickClick, 40));
			stickButton.setFilled(true);
			stickButton.arc(joyconRightEdgeX - (widthDivision / 2), stickCenterY, stickRadius, stickRadius, 0, 360);
			stickButton.close();

			minusPlusButton.setStrokeColor(getInputColor(currentInputValues.minusPlus));
			minusPlusButton.setStrokeWidth(2);
			minusPlusButton.setFilled(false);
			minusPlusButton.close();

			printHomeButton.setStrokeColor(ofColor(75, 75, 75));
			printHomeButton.setStrokeWidth(2);
			printHomeButton.setFillColor(getInputColor(currentInputValues.printHome));
			printHomeButton.setFilled(true);
			printHomeButton.close();

			srButton.setStrokeColor(ofColor(150, 150, 150, 200));
			srButton.setStrokeWidth(1);
			srButton.setFillColor(getInputColor(currentInputValues.sr));
			srButton.setFilled(true);
			srButton.close();

			slButton.setStrokeColor(ofColor(150, 150, 150, 200));
			slButton.setStrokeWidth(1);
			slButton.setFillColor(getInputColor(currentInputValues.sl));
			slButton.setFilled(true);
			slButton.close();

			float visualizationRadius = stickRadius * 1.75;
			float exceededLineWidth = (visualizationRadius + (fourButtonsRadius / 2));
			stickTargetArc.arc(stickVisualizationCenterX, stickCenterY, visualizationRadius, visualizationRadius, 0, 360);
			stickTargetArc.setStrokeColor(ofColor(150, 150, 150, 200));
			stickTargetArc.setStrokeWidth(3);
			stickTargetArc.setFilled(false);
			stickTargetArc.close();

			stickTargetLineUp.moveTo(stickVisualizationCenterX, stickCenterY);
			stickTargetLineUp.lineTo(stickVisualizationCenterX, stickCenterY - exceededLineWidth);
			stickTargetLineUp.close();
			stickTargetLineDown.moveTo(stickVisualizationCenterX, stickCenterY);
			stickTargetLineDown.lineTo(stickVisualizationCenterX, stickCenterY + exceededLineWidth);
			stickTargetLineDown.close();
			stickTargetLineLeft.moveTo(stickVisualizationCenterX, stickCenterY);
			stickTargetLineLeft.lineTo(stickVisualizationCenterX + exceededLineWidth, stickCenterY);
			stickTargetLineLeft.close();
			stickTargetLineRight.moveTo(stickVisualizationCenterX, stickCenterY);
			stickTargetLineRight.lineTo(stickVisualizationCenterX - exceededLineWidth, stickCenterY);
			stickTargetLineRight.close();

			stickTargetRadius = visualizationRadius;
			stickPointerRadius = stickTargetRadius / 6;
			stickPointerCenterX = stickVisualizationCenterX;
			stickPointerCenterY = stickCenterY;
			stickAsPolarX = stickPointerCenterX - stickTargetRadius;
			stickAsPolarY = stickCenterY - (stickTargetRadius + (2*BORDER));
		};

		void drawJoycon() {
			ofFill();
			ofSetColor(ofColor(joyconColor.r, joyconColor.g, joyconColor.b, 25));
			ofDrawRectangle(celPosX, celPosY, celWidth, celHeight); //background rect
			ofSetColor(ofColor(joyconColor.r, joyconColor.g, joyconColor.b, 50));
			ofDrawRectangle(celPosX + (BORDER / 2), celPosY + (BORDER / 2), celWidth - BORDER, celHeight - BORDER); //cel rect

			zlzrButton.setStrokeColor(ofColor(150, 150, 150, 200));
			zlzrButton.setStrokeWidth(1);
			zlzrButton.setFillColor(getInputColor(currentInputValues.zlzr));
			zlzrButton.setFilled(true);
			zlzrButton.draw();

			lrButton.setStrokeColor(ofColor(150, 150, 150, 200));
			lrButton.setStrokeWidth(1);
			lrButton.setFillColor(getInputColor(currentInputValues.lr));
			lrButton.setFilled(true);
			lrButton.draw();
		
			joyconDrawing.setStrokeColor(ofColor(255, 255, 255, 255));
			joyconDrawing.setStrokeWidth(1);
			joyconDrawing.setFillColor(joyconColor);
			joyconDrawing.setFilled(true);
			joyconDrawing.draw();

			upXButton.setFillColor(getInputColor(currentInputValues.upX));
			upXButton.setFilled(true);
			upXButton.draw();
			downBButton.setFillColor(getInputColor(currentInputValues.downB));
			downBButton.setFilled(true);
			downBButton.draw();
			leftYButton.setFillColor(getInputColor(currentInputValues.leftY));
			leftYButton.setFilled(true);
			leftYButton.draw();
			rightAButton.setFillColor(getInputColor(currentInputValues.rightA));
			rightAButton.setFilled(true);
			rightAButton.draw();

			stickBase.setFillColor(getInputColor(0));
			stickBase.setFilled(true);
			stickBase.draw();
			stickButton.setFillColor(getInputColor(currentInputValues.stickClick, 40));
			stickButton.setFilled(true);
			stickButton.draw();

			minusPlusButton.setStrokeColor(getInputColor(currentInputValues.minusPlus));
			minusPlusButton.setStrokeWidth(1);
			minusPlusButton.draw();

			printHomeButton.setStrokeColor(ofColor(75, 75, 75));
			printHomeButton.setStrokeWidth(2);
			printHomeButton.setFillColor(getInputColor(currentInputValues.printHome));
			printHomeButton.setFilled(true);
			printHomeButton.draw();

			srButton.setStrokeColor(ofColor(150, 150, 150, 200));
			srButton.setStrokeWidth(1);
			srButton.setFillColor(getInputColor(currentInputValues.sr));
			srButton.setFilled(true);
			srButton.draw();

			slButton.setStrokeColor(ofColor(150, 150, 150, 200));
			slButton.setStrokeWidth(1);
			slButton.setFillColor(getInputColor(currentInputValues.sl));
			slButton.setFilled(true);
			slButton.draw();

			if (celWidth >= minWidthForStick) {
				stickTargetArc.setStrokeColor(ofColor(150, 150, 150, 200));
				stickTargetArc.setStrokeWidth(3);
				stickTargetArc.setFilled(false);
				stickTargetArc.draw();

				stickTargetLineUp.setStrokeColor(getInputColor(currentInputValues.stickAsDpad.up, 100));
				stickTargetLineUp.setStrokeWidth(1);
				stickTargetLineUp.setFilled(false);
				stickTargetLineUp.draw();
				stickTargetLineDown.setStrokeColor(getInputColor(currentInputValues.stickAsDpad.down, 100));
				stickTargetLineDown.setStrokeWidth(1);
				stickTargetLineDown.setFilled(false);
				stickTargetLineDown.draw();
				stickTargetLineLeft.setStrokeColor(getInputColor(currentInputValues.stickAsDpad.left, 100));
				stickTargetLineLeft.setStrokeWidth(1);
				stickTargetLineLeft.setFilled(false);
				stickTargetLineLeft.draw();
				stickTargetLineRight.setStrokeColor(getInputColor(currentInputValues.stickAsDpad.right, 100));
				stickTargetLineRight.setStrokeWidth(1);
				stickTargetLineRight.setFilled(false);
				stickTargetLineRight.draw();

				float posX = currentInputValues.stickX * stickTargetRadius;
				float posY = currentInputValues.stickY * stickTargetRadius * -1;
				ofSetColor(joyconColor.r, joyconColor.g, joyconColor.b, joyconColor.a + 25);
				ofDrawCircle(stickPointerCenterX + posX, stickPointerCenterY + posY, stickPointerRadius); //stick pointer indicador

				ofSetColor(ofColor(150, 150, 150, 200));
				ofDrawBitmapString(ofToString(currentInputValues.stickDistance, 3), stickAsPolarX, stickAsPolarY - (2 * BORDER));
				ofDrawBitmapString(ofToString(currentInputValues.stickAzimuth, 3), stickAsPolarX, stickAsPolarY);
			}

			bool enoughSpaceForCookedGraph = true;
			if (dataGraphWidth < celWidth * 1 / 5 || !drawRawIMUData || !drawCookedIMUData) {
				dataGraphWidth = dataGraphWidth * 2;
				if(drawRawIMUData && drawCookedIMUData)
					enoughSpaceForCookedGraph = false;
			}
			
			if (drawRawIMUData && dataGraphWidth >= celWidth * 1 / 5) {
				float localDataGraphPosY = dataGraphPosY;
				float dataGraphHeight = (celHeight / 2) - localDataGraphPosY - BORDER;
				draw2DGraph(dataGraphPosX, celPosY + localDataGraphPosY, dataGraphWidth, dataGraphHeight, gyroXValues, gyroYValues, gyroZValues, MAX_GYRO_VALUE, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("angularVelocity") < dataGraphWidth && font.stringHeight("V") < dataGraphHeight)
					ofDrawBitmapString("angularVelocity", dataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
				else if (font.stringWidth("angularVelocity") >= dataGraphWidth && font.stringHeight("V") < dataGraphHeight)
					ofDrawBitmapString("aV", dataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);

				localDataGraphPosY = localDataGraphPosY + dataGraphHeight + BORDER;
				draw2DGraph(dataGraphPosX, celPosY + localDataGraphPosY, dataGraphWidth, dataGraphHeight, rawAccelXValues, rawAccelYValues, rawAccelZValues, MAX_ACCEL_VALUE, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("rawAccelerometer") < dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("rawAccelerometer", dataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
				else if (font.stringWidth("rawAccelerometer") >= dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("rA", dataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
			}

			if (drawCookedIMUData && enoughSpaceForCookedGraph) {
				float localDataGraphPosX = dataGraphPosX;
				if (drawRawIMUData)
					localDataGraphPosX = dataGraphPosX + dataGraphWidth + BORDER;
				float localDataGraphPosY = dataGraphPosY;
				float dataGraphHeight = (celHeight - (2 * localDataGraphPosY)) / 3 - BORDER;
				draw2DGraph(localDataGraphPosX, celPosY + localDataGraphPosY, dataGraphWidth, dataGraphHeight, quatIValues, quatJValues, quatKValues, 1, 4, quatWValues);
				ofSetColor(joyconColor);
				if (font.stringWidth("quatOrientation") < dataGraphWidth && font.stringHeight("O") < dataGraphHeight)
					ofDrawBitmapString("quatOrientation", localDataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
				else if (font.stringWidth("quatOrientation") >= dataGraphWidth && font.stringHeight("O") < dataGraphHeight)
					ofDrawBitmapString("qO", localDataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);

				localDataGraphPosY = localDataGraphPosY + dataGraphHeight + BORDER;
				draw2DGraph(localDataGraphPosX, celPosY + localDataGraphPosY, dataGraphWidth, dataGraphHeight, cookedAccelXValues, cookedAccelYValues, cookedAccelZValues, MAX_ACCEL_VALUE, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("accel-gravity") < dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("accel-gravity", localDataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
				else if (font.stringWidth("accel-gravity") >= dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("rA-g", localDataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);

				localDataGraphPosY = localDataGraphPosY + dataGraphHeight + BORDER;
				draw2DGraph(localDataGraphPosX, celPosY + localDataGraphPosY, dataGraphWidth, dataGraphHeight, gravityXValues, gravityYValues, gravityZValues, 1, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("gravity") < dataGraphWidth && font.stringHeight("G") < dataGraphHeight)
					ofDrawBitmapString("gravity", localDataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
				else if (font.stringWidth("gravity") >= dataGraphWidth && font.stringHeight("G") < dataGraphHeight)
					ofDrawBitmapString("grav", localDataGraphPosX, celPosY + localDataGraphPosY + BORDER * 2);
			}

			ofSetColor(ofColor(255, 255, 255, 255));
			float nameWidth = font.stringWidth(nameOnGUI);
			font.drawString(nameOnGUI, (celPosX + (2*BORDER)) + (abs(controllerType - 2) * (celWidth - (nameWidth + (4*BORDER)))), celPosY + dataGraphPosY);
		}

		void draw2DGraph(float posX, float posY, float graphWidth, float graphHeight, vector<float> graphValuesI, vector<float> graphValuesJ, vector<float> graphValuesK, float maxYValue = 1, int numLayers = 4, vector<float> graphValuesW = vector<float>()) {
			float xAxisStep = graphWidth / (IMUVectorsSize-1);
			float yAxisStep = (graphHeight / 2) / maxYValue;
			ofVec2f currentGraphPointI;
			ofVec2f currentGraphPointJ;
			ofVec2f currentGraphPointK;
			ofVec2f currentGraphPointW;
			ofVec2f lastGraphPointI;
			ofVec2f lastGraphPointJ;
			ofVec2f lastGraphPointK;
			ofVec2f lastGraphPointW;

			ofFill();
			ofSetColor(ofColor(5, 5, 5, 225));
			ofDrawRectangle(posX, posY, graphWidth, graphHeight);
			ofFill();
			ofSetColor(ofColor(joyconColor.r, joyconColor.g, joyconColor.b, 25));
			ofDrawRectangle(posX, posY, graphWidth, graphHeight); 
			ofSetLineWidth(0.5);
			ofSetColor(ofColor(150, 150, 150, 200));
			ofNoFill();
			ofDrawLine(posX, posY+(graphHeight/2), posX+graphWidth, posY+(graphHeight / 2));
			ofSetColor(ofColor(255, 255, 255, 200));
			ofNoFill();
			ofDrawRectangle(posX - 1, posY - 1, graphWidth + 2, graphHeight + 2); //draws background rect + line at 0

			ofSetLineWidth(0.5);
			posY = posY + (graphHeight / 2);
			for (int index = currentFirstPosGraphs, indexGraph = 0; indexGraph < IMUVectorsSize; index++, indexGraph++) { //_n7
				if (index > IMUVectorsSize - 1) {
					index = 0;
				}

				currentGraphPointI.x = posX + (indexGraph * xAxisStep);
				currentGraphPointI.y = posY + (graphValuesI[index] * yAxisStep);
				currentGraphPointJ.x = posX + (indexGraph * xAxisStep);
				currentGraphPointJ.y = posY + (graphValuesJ[index] * yAxisStep);
				currentGraphPointK.x = posX + (indexGraph * xAxisStep);
				currentGraphPointK.y = posY + (graphValuesK[index] * yAxisStep);
				if (numLayers > 3) {
					currentGraphPointW.x = posX + (indexGraph * xAxisStep);
					currentGraphPointW.y = posY + (graphValuesW[index] * yAxisStep);
				}

				if (indexGraph != 0) {
					ofFill();
					ofSetColor(GRAPH_RED);
					ofDrawLine(currentGraphPointI.x, currentGraphPointI.y, lastGraphPointI.x, lastGraphPointI.y);
					ofSetColor(GRAPH_GREEN);
					ofDrawLine(currentGraphPointJ.x, currentGraphPointJ.y, lastGraphPointJ.x, lastGraphPointJ.y);
					ofSetColor(GRAPH_BLUE);
					ofDrawLine(currentGraphPointK.x, currentGraphPointK.y, lastGraphPointK.x, lastGraphPointK.y);
					if (numLayers > 3) {
						ofSetColor(GRAPH_WHITE);
						ofDrawLine(currentGraphPointW.x, currentGraphPointW.y, lastGraphPointW.x, lastGraphPointW.y);
					}
				}

				lastGraphPointI.x = currentGraphPointI.x;
				lastGraphPointI.y = currentGraphPointI.y;
				lastGraphPointJ.x = currentGraphPointJ.x;
				lastGraphPointJ.y = currentGraphPointJ.y;
				lastGraphPointK.x = currentGraphPointK.x;
				lastGraphPointK.y = currentGraphPointK.y;
				if (numLayers > 3) {
					lastGraphPointW.x = currentGraphPointW.x;
					lastGraphPointW.y = currentGraphPointW.y;
				}
			}
		};

		ofColor getInputColor(int buttonValue, int baseRGB = BASE_BUTTON_COLOR) {
			ofColor inputColor = ofColor((buttonValue * RPRESSED) + baseRGB, (buttonValue * BPRESSED) + baseRGB, (buttonValue * GPRESSED) + baseRGB);
			return inputColor;
		};

		string checkMouseClick(int mouseClickX, int mouseClickY, int mouseButton) {
			bool insideJoycon = pointInsidePolylines(joyconDrawing.getOutline(), mouseClickX, mouseClickY);

			if (insideJoycon) {
				if (pointInsidePolylines(upXButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.upX, inputOSCTags.upX);
				else if (pointInsidePolylines(downBButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.downB, inputOSCTags.downB);
				else if (pointInsidePolylines(leftYButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.leftY, inputOSCTags.leftY);
				else if (pointInsidePolylines(rightAButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.rightA, inputOSCTags.rightA);
				else if (pointInsidePolylines(minusPlusButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.minusPlus, inputOSCTags.minusPlus);
				else if (pointInsidePolylines(printHomeButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.printHome, inputOSCTags.printHome);
				else if (pointInsidePolylines(stickButton.getOutline(), mouseClickX, mouseClickY)) 
					return mouseClickAction(mouseButton, currentInputValues.stickClick, inputOSCTags.stickClick);
			}
			else {
				if (pointInsidePolylines(stickTargetArc.getOutline(), mouseClickX, mouseClickY)) {
					if (mouseButton == OF_MOUSE_BUTTON_LEFT && isVirtual) {
						currentInputValues.stickX = ofMap(mouseClickX - stickPointerCenterX, -1 * stickTargetRadius, stickTargetRadius, -1, 1);
						currentInputValues.stickY = ofMap(mouseClickY - stickPointerCenterY, stickTargetRadius, -1 * stickTargetRadius, -1, 1); 
						oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickX, currentInputValues.stickX));
						oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickY, currentInputValues.stickY));
						if (useStickAsPolar) {
							ofVec2f newPolar = getStickAsPolarCoordinates(currentInputValues.stickX, currentInputValues.stickY);
							currentInputValues.stickDistance = newPolar.x;
							currentInputValues.stickAzimuth = newPolar.y;
							oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickDistance, currentInputValues.stickDistance));
							oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAzimuth, currentInputValues.stickAzimuth));
						}
						if (useStickAsDpad) {
							stickAsDpad newStickAsDpad = setStickAsDpad(currentInputValues.stickX, currentInputValues.stickY);
							if (newStickAsDpad.up != currentInputValues.stickAsDpad.up)
								oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadUp, newStickAsDpad.up));
							if (newStickAsDpad.down != currentInputValues.stickAsDpad.down)
								oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadDown, newStickAsDpad.down));
							if (newStickAsDpad.left != currentInputValues.stickAsDpad.left)
								oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadLeft, newStickAsDpad.left));
							if (newStickAsDpad.right != currentInputValues.stickAsDpad.right)
								oscSender.sendMessage(getInputOscMessage(inputOSCTags.stickAsDpadRight, newStickAsDpad.right));
							currentInputValues.stickAsDpad = newStickAsDpad;
						}
						return "";
					}
					else {
						string valueRangeMessage = "from -1.0 to 1.0 ";
						string valueMessagePolar = "polarCoord:";
						string valueMessageDpad = "stickAsDpad:";
						return valueMessagePolar + '\n' +
							joyconOscAddress + inputOSCTags.stickDistance + '\n' +
							joyconOscAddress + inputOSCTags.stickAzimuth + '\n' + '\n' +
							valueRangeMessage + '\n' +
							joyconOscAddress + inputOSCTags.stickX + '\n' + 
							joyconOscAddress + inputOSCTags.stickY + '\n' + '\n' + 
							valueMessageDpad + '\n' +
							joyconOscAddress + inputOSCTags.stickAsDpadUp + '\n' +
							joyconOscAddress + inputOSCTags.stickAsDpadDown + '\n' +
							joyconOscAddress + inputOSCTags.stickAsDpadLeft + '\n' +
							joyconOscAddress + inputOSCTags.stickAsDpadRight;
					}
				}
				else if ((mouseClickX > stickPointerCenterX && controllerType == JS_TYPE_JOYCON_LEFT) || (mouseClickX < stickPointerCenterX && controllerType == JS_TYPE_JOYCON_RIGHT)) {
					if (pointInsidePolylines(lrButton.getOutline(), mouseClickX, mouseClickY))
						return mouseClickAction(mouseButton, currentInputValues.lr, inputOSCTags.lr);
					else if (pointInsidePolylines(zlzrButton.getOutline(), mouseClickX, mouseClickY)) 
						return mouseClickAction(mouseButton, currentInputValues.zlzr, inputOSCTags.zlzr);
					else if (pointInsidePolylines(slButton.getOutline(), mouseClickX, mouseClickY))
						return mouseClickAction(mouseButton, currentInputValues.sl, inputOSCTags.sl);
					else if (pointInsidePolylines(srButton.getOutline(), mouseClickX, mouseClickY)) 
						return mouseClickAction(mouseButton, currentInputValues.sr, inputOSCTags.sr);
				}
				else {
					if (mouseClickX >= dataGraphPosX && mouseClickX <= dataGraphPosX + dataGraphWidth) {
						float localDataGraphPosY = celPosY + dataGraphPosY;
						float rawIMUGraphWidth = (celHeight / 2) - dataGraphPosY - BORDER;
						if (mouseClickY >= localDataGraphPosY && mouseClickY <= localDataGraphPosY + rawIMUGraphWidth) {
							return "from " + ofToString(-1 * MAX_GYRO_VALUE) + " to " + ofToString(MAX_GYRO_VALUE) + " dps" + '\n' +
								joyconOscAddress + inputOSCTags.gyroX + '\n' + 
								joyconOscAddress + inputOSCTags.gyroY + '\n' + 
								joyconOscAddress + inputOSCTags.gyroZ;
						}
						else if (mouseClickY >= localDataGraphPosY + rawIMUGraphWidth && mouseClickY <= localDataGraphPosY + (2 * rawIMUGraphWidth)) {
							return "from " + ofToString(-1 * MAX_ACCEL_VALUE) + " to " + ofToString(MAX_ACCEL_VALUE) + " g" + '\n' +
								joyconOscAddress + inputOSCTags.raclX + '\n' +
								joyconOscAddress + inputOSCTags.raclY + '\n' +
								joyconOscAddress + inputOSCTags.raclZ;
						}
					}
					else if (mouseClickX >= dataGraphPosX + dataGraphWidth && mouseClickX <= dataGraphPosX + (2 * dataGraphWidth)) {
						float localDataGraphPosY = celPosY + dataGraphPosY;
						float cokIMUGraphWidth = (celHeight - (2 * dataGraphPosY)) / 3 - BORDER;
						string valueRangeMessage = "from -1.0 to 1.0 ";
						if (mouseClickY >= localDataGraphPosY && mouseClickY <= localDataGraphPosY + cokIMUGraphWidth) {
							return "quaternions " + valueRangeMessage + '\n' +
								joyconOscAddress + inputOSCTags.quatW + '\n' +
								joyconOscAddress + inputOSCTags.quatX + '\n' +
								joyconOscAddress + inputOSCTags.quatY + '\n' +
								joyconOscAddress + inputOSCTags.quatZ;
						}
						else if (mouseClickY >= localDataGraphPosY + cokIMUGraphWidth && mouseClickY <= localDataGraphPosY + (2 * cokIMUGraphWidth)) {
							return "from " + ofToString(-1 * MAX_ACCEL_VALUE) + " to " + ofToString(MAX_ACCEL_VALUE) + " g" + '\n' +
								joyconOscAddress + inputOSCTags.caclX + '\n' +
								joyconOscAddress + inputOSCTags.caclY + '\n' +
								joyconOscAddress + inputOSCTags.caclZ;
						}
						else if (mouseClickY >= localDataGraphPosY + (2 * cokIMUGraphWidth) && mouseClickY <= localDataGraphPosY + (3 * cokIMUGraphWidth) + BORDER) {
							return valueRangeMessage + '\n' +
								joyconOscAddress + inputOSCTags.gravX + '\n' +
								joyconOscAddress + inputOSCTags.gravY + '\n' +
								joyconOscAddress + inputOSCTags.gravZ;
						}
					}
				}
			}

			return "";
		}

		bool pointInsidePolylines(vector<ofPolyline> polylinesToCheck, int pointX, int pointY) {
			for each (ofPolyline polyline in polylinesToCheck) {
				if (polyline.inside(pointX, pointY)) {
					return true;
				}
			}
			return false;
		}

		string mouseClickAction(int mouseButton, bool &joyconButton, string joyconButtonOSCTag) {
			if (mouseButton == OF_MOUSE_BUTTON_LEFT && isVirtual) {
				joyconButton = true;
				clickedInputPointer = &joyconButton;
				return joyconButtonOSCTag;
			}
			else {
				return joyconOscAddress + joyconButtonOSCTag;
			}
		}
};

/*
	n1- those 3 constants represent the color of the pressed buttons in rgb;
	n2- this is used with the constants defined on JoyShockLibrary. This library also identifies
		ProControllers(3) and DualShock(4), but that's not suported;
	n3- Virtual joycons are objects of type 'Joycon' that are not linked to connected divices. They are just
		visualizations used to test and configure preferences of each joycon input even when no 
		devices are connected;
	n4- these float vectors are used to store the last 'IMUVectorsSize' values of IMU data, raw and cooked.
		This is used to draw the graphs with the data;
	n5- the stats/values of all joycon buttons come in a single integer number, with each bit of the integer
		corresponding to a button. The correct bits for each button are here JSMASK constants, defined
		on JoyShockLibrary. If a button is pressed the value should be 1, if not, 0;
	n6- this if-else section sets the variables used to draw the joycon, changing positions based on controller type
		(that's why some ofPaths have a composite name);
	n7- the logic on the 2D graph construction is kind of diferent from a common 'for' loop. The ideia here is to make
		the new IMU value added each frame to the vectors always apear on the end of the 2D graph. For that, it reads
		the vectors from position 'currentFirstPosGraphs' to 'currentFirstPosGraphs-1', changing 'currentFirstPosGraphs'
		every frame to 'the index where was added the new IMU value in the vector' + 1.
*/