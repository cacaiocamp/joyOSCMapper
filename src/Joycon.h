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
#define MIN_WIDTH_GRAPH_TEXT 100

class Joycon {
	public:
		int deviceId = -1;
		int intColor = 0xffffff;
		int controllerType; //left(1) or right(2) joycon identifier _n2
		inputValues currentInputValues;
		IMU_STATE rawIMUData = IMU_STATE();
		MOTION_STATE cookedIMUData = MOTION_STATE();
		bool isVirtual = false; //_n3
		int positionOnList;

		ofColor joyconColor;
		ofxToggle GUIToggle;
		string nameOnGUI;
		ofTrueTypeFont font; 
		ofxOscSender oscSender;
		string oscNetAddress = "localhost";
		int oscSendPort = 12345;
		string joyconOscAddress = "localhost";
		inputOSCTags inputOSCTags;

		int celPosX = 0;
		int celPosY = 0;
		bool useRawIMUData = true;
		bool useCookedIMUData = true;
		bool drawRawIMUData = true;
		bool drawCookedIMUData = true;
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
		int IMUVectorsSize = 70;
		int currentFirstPosGraphs = 0; //_n4

		float minStickStep = 0.0025;

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

			joyconOscAddress = prefix + ofToString(devicesConnectedNumber);
		}

		void oscSenderSetup() {
			oscSender.setup(oscNetAddress, oscSendPort);
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
			inputValues currentInputValues;

			if (controllerType == JS_TYPE_JOYCON_LEFT) {
				currentInputValues.upX = (buttonsData & JSMASK_UP) == JSMASK_UP;
				currentInputValues.downB = (buttonsData & JSMASK_DOWN) == JSMASK_DOWN;
				currentInputValues.leftY = (buttonsData & JSMASK_LEFT) == JSMASK_LEFT;
				currentInputValues.rightA = (buttonsData & JSMASK_RIGHT) == JSMASK_RIGHT;
				currentInputValues.minusPlus = (buttonsData & JSMASK_MINUS) == JSMASK_MINUS;
				currentInputValues.stickClick = (buttonsData & JSMASK_LCLICK) == JSMASK_LCLICK;
				currentInputValues.lr = (buttonsData & JSMASK_L) == JSMASK_L;
				currentInputValues.zlzr = (buttonsData & JSMASK_ZL) == JSMASK_ZL;
				currentInputValues.printHome = (buttonsData & JSMASK_CAPTURE) == JSMASK_CAPTURE;
				currentInputValues.stickX = newButtonsStickData.stickLX;
				currentInputValues.stickY = newButtonsStickData.stickLY;
			}
			else {
				currentInputValues.upX = (buttonsData & JSMASK_N) == JSMASK_N;
				currentInputValues.downB = (buttonsData & JSMASK_S) == JSMASK_S;
				currentInputValues.leftY = (buttonsData & JSMASK_W) == JSMASK_W;
				currentInputValues.rightA = (buttonsData & JSMASK_E) == JSMASK_E;
				currentInputValues.minusPlus = (buttonsData & JSMASK_PLUS) == JSMASK_PLUS;
				currentInputValues.stickClick = (buttonsData & JSMASK_RCLICK) == JSMASK_RCLICK;
				currentInputValues.lr = (buttonsData & JSMASK_R) == JSMASK_R;
				currentInputValues.zlzr = (buttonsData & JSMASK_ZR) == JSMASK_ZR;
				currentInputValues.printHome = (buttonsData & JSMASK_HOME) == JSMASK_HOME;
				currentInputValues.stickX = newButtonsStickData.stickRX;
				currentInputValues.stickY = newButtonsStickData.stickRY;
			}
			currentInputValues.sl = (buttonsData & JSMASK_SL) == JSMASK_SL;
			currentInputValues.sr = (buttonsData & JSMASK_SR) == JSMASK_SR;

			return currentInputValues;
		}

		void sendNewInputsAsOSC(inputValues newInputValues) {
			if (newInputValues.upX != currentInputValues.upX)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.upX, newInputValues.upX));
			if (newInputValues.downB != currentInputValues.downB)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.downB, newInputValues.downB));
			if (newInputValues.leftY != currentInputValues.leftY)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.leftY, newInputValues.leftY));
			if (newInputValues.rightA != currentInputValues.rightA)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.rightA, newInputValues.rightA));
			if (newInputValues.minusPlus != currentInputValues.minusPlus)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.minusPlus, newInputValues.minusPlus));
			if (newInputValues.stickClick != currentInputValues.stickClick)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.stickClick, newInputValues.stickClick));
			if (newInputValues.lr != currentInputValues.lr)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.lr, newInputValues.lr));
			if (newInputValues.zlzr != currentInputValues.zlzr)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.zlzr, newInputValues.zlzr));
			if (newInputValues.printHome != currentInputValues.printHome)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.printHome, newInputValues.printHome));
			if (newInputValues.sl != currentInputValues.sl)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.sl, newInputValues.sl));
			if (newInputValues.sr != currentInputValues.sr)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.sr, newInputValues.sr));
			if (abs(newInputValues.stickX - currentInputValues.stickX) >= minStickStep)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.stickX, newInputValues.stickX));
			if (abs(newInputValues.stickY - currentInputValues.stickY) >= minStickStep)
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.stickY, newInputValues.stickY));

			if (useRawIMUData) {
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.gyroX, rawIMUData.gyroX));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.gyroY, rawIMUData.gyroY));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.gyroZ, rawIMUData.gyroZ));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.raclX, rawIMUData.accelX));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.raclY, rawIMUData.accelY));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.raclZ, rawIMUData.accelZ));
			}
			if (useCookedIMUData) {
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.quatW, cookedIMUData.quatW));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.quatX, cookedIMUData.quatX));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.quatY, cookedIMUData.quatY));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.quatZ, cookedIMUData.quatZ));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.caclX, cookedIMUData.accelX));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.caclY, cookedIMUData.accelY));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.caclZ, cookedIMUData.accelZ));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.gravX, cookedIMUData.gravX));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.gravY, cookedIMUData.gravY));
				oscSender.sendMessage(getInputOscMessage(joyconOscAddress, inputOSCTags.gravZ, cookedIMUData.gravZ));
			}
		}

		ofxOscMessage getInputOscMessage(string oscNetAddress, string inputAddress, float inputValue) {
			ofxOscMessage inputMessage;
			inputMessage.setAddress(oscNetAddress + inputAddress);
			inputMessage.addFloatArg(inputValue);
			return inputMessage;
		};

		void updateGraphsVisualizations() {
			if (isVirtual) {
				gyroXValues[currentFirstPosGraphs] = ofRandom(-MAX_GYRO_VALUE, MAX_GYRO_VALUE) * -1;
				gyroYValues[currentFirstPosGraphs] = ofRandom(-MAX_GYRO_VALUE, MAX_GYRO_VALUE) * -1;
				gyroZValues[currentFirstPosGraphs] = ofRandom(-MAX_GYRO_VALUE, MAX_GYRO_VALUE) * -1;
				rawAccelXValues[currentFirstPosGraphs] = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE) * -1;
				rawAccelYValues[currentFirstPosGraphs] = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE) * -1;
				rawAccelZValues[currentFirstPosGraphs] = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE) * -1;
				quatWValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
				quatIValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
				quatJValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
				quatKValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
				cookedAccelXValues[currentFirstPosGraphs] = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE) * -1;
				cookedAccelYValues[currentFirstPosGraphs] = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE) * -1;
				cookedAccelZValues[currentFirstPosGraphs] = ofRandom(-MAX_ACCEL_VALUE, MAX_ACCEL_VALUE) * -1;
				gravityXValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
				gravityYValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
				gravityZValues[currentFirstPosGraphs] = ofRandom(-1, 1) * -1;
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

		void drawJoycon(int celWidth, int celHeight, int border) {
			ofFill();
			ofSetColor(ofColor(joyconColor.r, joyconColor.g, joyconColor.b, 25));
			ofDrawRectangle(celPosX, celPosY, celWidth, celHeight); //background rect
			ofSetColor(ofColor(joyconColor.r, joyconColor.g, joyconColor.b, 50));
			ofDrawRectangle(celPosX + (border / 2), celPosY + (border / 2), celWidth - border, celHeight - border); //cel rect

			float heightDivision = celHeight / 9;
			float widthDivision = heightDivision * 2.25;
			float fourButtonsRadius = heightDivision / 4;
			float stickRadius = heightDivision / 2.5;
			float horizontalBorder = (border*2) + (fourButtonsRadius / 3);
			float textProportion = float(celWidth) / float(celHeight);
			if (textProportion < 3)
				textProportion = 3;

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
			ofPath minusPlusButton;
			ofPath homePrintButton;
			ofPath srButton;
			ofPath slButton;
			ofPath lrButton;
			ofPath zlzrButton;

			ofPath stickVisualization;
			float stickVisualizationCenterX = 0;

			float dataGraphPosX = 0;
			float dataGraphWidth = 0;

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

				minusPlusButton.moveTo(joyconRightEdgeX - (widthDivision / 4) , joyconRightEdgeUpperY + (heightDivision/2));
				minusPlusButton.lineTo(joyconRightEdgeX - (widthDivision / 4) + (1.25*fourButtonsRadius), joyconRightEdgeUpperY + (heightDivision / 2));
				homePrintButton.rectangle(joyconRightEdgeX - (widthDivision / 2.5), joyconLeftEdgeLowerY - (heightDivision / 1.5), 1.5*fourButtonsRadius, 1.5 * fourButtonsRadius);
				srButton.rectangle(joyconRightEdgeX + 1, fourButtonsCenterY + (2*fourButtonsRadius), (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				slButton.rectangle(joyconRightEdgeX + 1, stickCenterY - fourButtonsRadius, (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				lrButton.arc(joyconCurvesCenterX - (fourButtonsRadius / 1.5), celPosY + (heightDivision * 2) - (fourButtonsRadius / 1.5), widthDivision - (fourButtonsRadius / 1.5), heightDivision - (fourButtonsRadius / 2), upperCurveAngleBegin, upperCurveAngleBegin + 90);
				zlzrButton.arc(joyconCurvesCenterX - (widthDivision / 2.325), celPosY + heightDivision + (heightDivision / 1.5), widthDivision/1.75, heightDivision/1.5, upperCurveAngleBegin, upperCurveAngleBegin + 90);

				stickVisualizationCenterX = joyconLeftEdgeX - horizontalBorder - (stickRadius * 1.75);
				dataGraphPosX = celPosX + (2 * border);
				dataGraphWidth = ((stickVisualizationCenterX - border - (stickRadius * 1.75) - horizontalBorder - celPosX)/2) - border;
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

				minusPlusButton.moveTo(joyconLeftEdgeX + (widthDivision / 4), joyconLeftEdgeUpperY + (heightDivision / 2));
				minusPlusButton.lineTo(joyconLeftEdgeX + (widthDivision / 4) - (1.25*fourButtonsRadius), joyconLeftEdgeUpperY + (heightDivision / 2));
				minusPlusButton.moveTo(joyconLeftEdgeX + (widthDivision / 4) - (0.625*fourButtonsRadius), joyconLeftEdgeUpperY + (heightDivision/2) + (0.625*fourButtonsRadius));
				minusPlusButton.lineTo(joyconLeftEdgeX + (widthDivision / 4) - (0.625*fourButtonsRadius), joyconLeftEdgeUpperY + (heightDivision / 2) - (0.625*fourButtonsRadius));
				homePrintButton.circle(joyconLeftEdgeX + (widthDivision / 2.5), joyconRightEdgeLowerY - (heightDivision / 2), fourButtonsRadius);
				srButton.rectangle(joyconLeftEdgeX - (fourButtonsRadius/3) - 1, fourButtonsCenterY - fourButtonsRadius, (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				slButton.rectangle(joyconLeftEdgeX - (fourButtonsRadius/3) - 1, stickCenterY + (2 * fourButtonsRadius), (fourButtonsRadius / 3), 2.25*fourButtonsRadius);
				lrButton.arc(joyconCurvesCenterX + (fourButtonsRadius / 1.5), celPosY + (heightDivision * 2) - (fourButtonsRadius / 1.5), widthDivision - (fourButtonsRadius / 1.5), heightDivision - (fourButtonsRadius / 2), upperCurveAngleBegin, upperCurveAngleBegin + 90);
				zlzrButton.arc(joyconCurvesCenterX + (widthDivision / 2.325), celPosY + heightDivision + (heightDivision / 1.5), widthDivision / 1.75, heightDivision / 1.5, upperCurveAngleBegin, upperCurveAngleBegin + 90);

				stickVisualizationCenterX = joyconRightEdgeX + horizontalBorder + (stickRadius * 1.75);
				dataGraphPosX = stickVisualizationCenterX + (stickRadius * 1.75) + horizontalBorder;
				dataGraphWidth = ((celWidth - (stickVisualizationCenterX + (stickRadius * 1.75) + border + horizontalBorder - celPosX))/2) - border;
			}

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

			ofPath joyconDrawing;
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
			joyconDrawing.draw();//joycon borders

			ofSetColor(ofColor(255, 255, 255, 255));
			float nameWidth = font.stringWidth(nameOnGUI);
			if (nameWidth > celWidth / 2) 
				font.drawString("J" + ofToString(positionOnList), joyconLeftEdgeX, celPosY + heightDivision / 2);
			else
				font.drawString(nameOnGUI, joyconLeftEdgeX + ((controllerType - 2) * (nameWidth - widthDivision)), celPosY + heightDivision/2);

			ofSetColor(getInputColor(currentInputValues.upX));
			ofDrawCircle(joyconRightEdgeX - (widthDivision / 2), fourButtonsCenterY - (heightDivision / 2), fourButtonsRadius); //up button
			ofSetColor(getInputColor(currentInputValues.downB));
			ofDrawCircle(joyconRightEdgeX - (widthDivision / 2), fourButtonsCenterY + (heightDivision / 2), fourButtonsRadius); //down button
			ofSetColor(getInputColor(currentInputValues.leftY));
			ofDrawCircle(joyconRightEdgeX - (widthDivision / 2) - (fourButtonsRadius * 2), fourButtonsCenterY, fourButtonsRadius); //left button
			ofSetColor(getInputColor(currentInputValues.rightA));
			ofDrawCircle(joyconRightEdgeX - (widthDivision / 2) + (fourButtonsRadius * 2), fourButtonsCenterY, fourButtonsRadius); //right button

			ofSetColor(ofColor(25, 25, 25));
			ofDrawCircle(joyconRightEdgeX - (widthDivision / 2), stickCenterY, stickRadius + (stickRadius / 3)); //stick base
			ofSetColor(getInputColor(currentInputValues.stickClick, 40));
			ofDrawCircle(joyconRightEdgeX - (widthDivision / 2), stickCenterY, stickRadius); //stick

			minusPlusButton.setStrokeColor(getInputColor(currentInputValues.minusPlus));
			minusPlusButton.setStrokeWidth(3);
			minusPlusButton.draw();

			homePrintButton.setStrokeColor(ofColor(75, 75, 75));
			homePrintButton.setStrokeWidth(2);
			homePrintButton.setFillColor(getInputColor(currentInputValues.printHome));
			homePrintButton.setFilled(true);
			homePrintButton.draw();

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

			if (celWidth >= 2 * widthDivision) {
				float visualizationRadius = stickRadius * 1.75;
				float exceededLineWidth = (visualizationRadius + (fourButtonsRadius / 2));
				stickVisualization.arc(stickVisualizationCenterX, stickCenterY, visualizationRadius, visualizationRadius, 0, 360);
				stickVisualization.setStrokeColor(ofColor(150, 150, 150, 200));
				stickVisualization.setStrokeWidth(3);
				stickVisualization.setFilled(false);
				stickVisualization.draw();
				stickVisualization.moveTo(stickVisualizationCenterX, stickCenterY - exceededLineWidth);
				stickVisualization.lineTo(stickVisualizationCenterX, stickCenterY + exceededLineWidth);
				stickVisualization.moveTo(stickVisualizationCenterX - exceededLineWidth, stickCenterY);
				stickVisualization.lineTo(stickVisualizationCenterX + exceededLineWidth, stickCenterY);
				stickVisualization.setStrokeWidth(1);
				stickVisualization.setFilled(false);
				stickVisualization.draw();

				float posX = currentInputValues.stickX * visualizationRadius;
				float posY = currentInputValues.stickY * visualizationRadius * -1;

				ofSetColor(joyconColor);
				ofDrawCircle(stickVisualizationCenterX + posX, stickCenterY + posY, fourButtonsRadius / 2); //stick position indicador
			}

			bool enoughSpaceForCookedGraph = true;
			if (dataGraphWidth < celWidth * 1 / 5) {
				enoughSpaceForCookedGraph = false;
				dataGraphWidth = dataGraphWidth * 2;
			}

			if (drawRawIMUData && dataGraphWidth >= celWidth * 1 / 5) {
				float dataGraphPosY = (heightDivision/2) + border;
				float dataGraphHeight = (celHeight/2) - dataGraphPosY - border;
				draw2DGraph(dataGraphPosX, celPosY + dataGraphPosY, dataGraphWidth, dataGraphHeight, gyroXValues, gyroYValues, gyroZValues, MAX_GYRO_VALUE, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("angularVelocity") < dataGraphWidth && font.stringHeight("V") < dataGraphHeight)
					ofDrawBitmapString("angularVelocity", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
				else if (font.stringWidth("angularVelocity") >= dataGraphWidth && font.stringHeight("V") < dataGraphHeight)
					ofDrawBitmapString("aV", dataGraphPosX, celPosY + dataGraphPosY + border * 2);

				dataGraphPosY = dataGraphPosY + dataGraphHeight + border;
				draw2DGraph(dataGraphPosX, celPosY + dataGraphPosY, dataGraphWidth, dataGraphHeight, rawAccelXValues, rawAccelYValues, rawAccelZValues, MAX_ACCEL_VALUE, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("rawAccelerometer") < dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("rawAccelerometer", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
				else if (font.stringWidth("rawAccelerometer") >= dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("rA", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
			}

			if (drawCookedIMUData && enoughSpaceForCookedGraph) {
				dataGraphPosX = dataGraphPosX + dataGraphWidth + border;
				float dataGraphPosY = (heightDivision / 2) + border;
				float dataGraphHeight = (celHeight - (2 * dataGraphPosY)) / 3 - border;
				draw2DGraph(dataGraphPosX, celPosY + dataGraphPosY, dataGraphWidth, dataGraphHeight, quatIValues, quatJValues, quatKValues, 1, 4, quatWValues);
				ofSetColor(joyconColor);
				if (font.stringWidth("quatOrientation") < dataGraphWidth && font.stringHeight("O") < dataGraphHeight)
					ofDrawBitmapString("quatOrientation", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
				else if(font.stringWidth("quatOrientation") >= dataGraphWidth && font.stringHeight("O") < dataGraphHeight)
					ofDrawBitmapString("qO", dataGraphPosX, celPosY + dataGraphPosY + border * 2);

				dataGraphPosY = dataGraphPosY + dataGraphHeight + border;
				draw2DGraph(dataGraphPosX, celPosY + dataGraphPosY, dataGraphWidth, dataGraphHeight, cookedAccelXValues, cookedAccelYValues, cookedAccelZValues, MAX_ACCEL_VALUE, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("accel-gravity") < dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("accel-gravity", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
				else if (font.stringWidth("accel-gravity") >= dataGraphWidth && font.stringHeight("A") < dataGraphHeight)
					ofDrawBitmapString("rA-g", dataGraphPosX, celPosY + dataGraphPosY + border * 2);

				dataGraphPosY = dataGraphPosY + dataGraphHeight + border;
				draw2DGraph(dataGraphPosX, celPosY + dataGraphPosY, dataGraphWidth, dataGraphHeight, gravityXValues, gravityYValues, gravityZValues, 1, 3);
				ofSetColor(joyconColor);
				if (font.stringWidth("gravity") < dataGraphWidth && font.stringHeight("G") < dataGraphHeight)
					ofDrawBitmapString("gravity", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
				else if (font.stringWidth("gravity") >= dataGraphWidth && font.stringHeight("G") < dataGraphHeight)
					ofDrawBitmapString("grav", dataGraphPosX, celPosY + dataGraphPosY + border * 2);
			}
		}

		void draw2DGraph(float posX, float posY, float graphWidth, float graphHeight, vector<float> graphValuesI, vector<float> graphValuesJ, vector<float> graphValuesK, float maxYValue = 1, int numLayers = 4, vector<float> graphValuesW = vector<float>()) {
			float xAxisStep = graphWidth / (IMUVectorsSize-1);
			float yAxisStep = (graphHeight / 2) / maxYValue;
			int lastPosition = currentFirstPosGraphs - 1;
			ofVec2f currentGraphPointI;
			ofVec2f currentGraphPointJ;
			ofVec2f currentGraphPointK;
			ofVec2f currentGraphPointW;
			ofVec2f lastGraphPointI;
			ofVec2f lastGraphPointJ;
			ofVec2f lastGraphPointK;
			ofVec2f lastGraphPointW;

			if (lastPosition < 0) {
				lastPosition = IMUVectorsSize - 1;
			}

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
					ofSetColor(ofColor(255, 100, 100, 200));
					ofDrawLine(currentGraphPointI.x, currentGraphPointI.y, lastGraphPointI.x, lastGraphPointI.y);
					ofSetColor(ofColor(0, 255, 0, 200));
					ofDrawLine(currentGraphPointJ.x, currentGraphPointJ.y, lastGraphPointJ.x, lastGraphPointJ.y);
					ofSetColor(ofColor(50, 100, 255, 200));
					ofDrawLine(currentGraphPointK.x, currentGraphPointK.y, lastGraphPointK.x, lastGraphPointK.y);
					if (numLayers > 3) {
						ofSetColor(ofColor(200, 200, 200, 200));
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
};

/*
	n1- those 3 constants represent the color of the pressed buttons in rgb;
	n2- this is used with the constants defined on JoyShockLibrary. This library also identifies
		ProControllers(3) and DualShock(4), but that's not suported;
	n3- Virtual joycons are objects of type 'Joycon' that are not linked to connected divices. They are just
		visualizations used to test, configure and save the OSC messages sent by each joycon input even when no 
		device is connected;
	n4- these float vectors are used to store the last 'IMUVectorsSize' values of IMU data, raw and cooked.
		This is used to draw the graphs with the data;
	n5- the stats/values of all joycon buttons come in a single integer number, with each bit of the integer
		corresponding to a button. The correct bits for each button are here JSMASK constants, defined
		on JoyShockLibrary. If a button is pressed, the value should be 1, if not, 0;
	n6- this if-else section sets the variables used to draw the joycon, changing positions based on controller type
		(that's why some ofPaths have a composite name). Right after, all joycon components are drawn;
	n7- the logic on the 2D graph construction is kind of diferent from a common 'for' loop. The ideia here is to make
		the new IMU value added each frame to the vectors always apear on the end of the 2D graph. For that, it reads
		the vectors from position 'currentFirstPosGraphs' to 'currentFirstPosGraphs-1', changing 'currentFirstPosGraphs'
		every frame to 'the index where was added the new IMU value in the vector' + 1.
*/