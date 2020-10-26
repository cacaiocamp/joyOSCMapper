#include "ofApp.h"
#include "JoyShockLibrary.h"

ofApp *joyOSCMapperPointer;
void getJslCallback(int joyconId, JOY_SHOCK_STATE lastButtonsStickData, JOY_SHOCK_STATE newButtonsStickData, IMU_STATE lastRawIMUData, IMU_STATE newRawIMUData, float secsBetweenReports) {
	joyOSCMapperPointer->updateJoyconData(joyconId, newButtonsStickData, newRawIMUData);
}

//--------------------------------------------------------------
void ofApp::setup(){
	joyOSCMapperPointer = this;//_n1

	//set dimensions of the App window
	int screenWidth = ofGetScreenWidth();
	int screenHeight = ofGetScreenHeight();
	int winWidthProportion = 5;
	int winHeightProportion = 3;
	int hMargin = screenWidth / winWidthProportion;
	int vMargin = screenHeight / winHeightProportion;
	winWidth = screenWidth - (2 * hMargin);
	winHeight = screenHeight - (2 * vMargin);

	ofSetWindowShape(winWidth, winHeight);
	ofSetWindowPosition(hMargin, vMargin);
	ofSetWindowTitle("joyOSCMapper");
	ofSetFrameRate(70);
	font.load(OF_TTF_MONO, 10);

	setupGuiControl();
	instantiateConnectedJoycons();
	setupGuiJoyconsList();

	guiShortcuts.setup();
	guiShortcuts.setName("Shortcuts/Help");
	guiShortcuts.add(sShortcut.setup("s-show/hide Shortcuts    ", vShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(cShortcut.setup("c-openGeneralConfig      ", vShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(gShortcut.setup("g-show/hide GUIControl   ", gShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(uShortcut.setup("  u-updateConnected      ", uShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(dShortcut.setup("  d-disconnectDisposeAll ", uShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(jShortcut.setup("j-show/hide JoyconsList  ", jShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(vShortcut.setup("v-un/toggle VirtualJoys  ", vShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(aShortcut.setup("  a-addAVirualJoycon     ", vShortcut, guiWidth, guiLineHeight));
	guiShortcuts.add(rShortcut.setup("  r-removeAVirualJoycon  ", vShortcut, guiWidth, guiLineHeight));
	guiShortcuts.setBackgroundColor(guiColor);
	sShortcut.setBackgroundColor(guiColor);
	cShortcut.setBackgroundColor(guiColor);
	dShortcut.setBackgroundColor(guiColor);
	gShortcut.setBackgroundColor(guiColor);
	jShortcut.setBackgroundColor(guiColor);
	uShortcut.setBackgroundColor(guiColor);
	vShortcut.setBackgroundColor(guiColor);
	aShortcut.setBackgroundColor(guiColor);
	rShortcut.setBackgroundColor(guiColor);

	guiGraphHelp.setup();
	guiGraphHelp.setName("GraphColors");
	guiGraphHelp.add(wColorHelp.setup("  w-quaternion           ", vShortcut, guiWidth, guiLineHeight));
	guiGraphHelp.add(xiColorHelp.setup("  x/i-quaternion         ", vShortcut, guiWidth, guiLineHeight));
	guiGraphHelp.add(yjColorHelp.setup("  y/j-quaternion         ", vShortcut, guiWidth, guiLineHeight));
	guiGraphHelp.add(zkColorHelp.setup("  z/k-quaternion         ", vShortcut, guiWidth, guiLineHeight));
	guiGraphHelp.setBackgroundColor(guiColor);
	wColorHelp.setBackgroundColor(ofColor(200, 200, 200, 150));
	xiColorHelp.setBackgroundColor(ofColor(255, 100, 100, 150));
	yjColorHelp.setBackgroundColor(ofColor(0, 255, 0, 150));
	zkColorHelp.setBackgroundColor(ofColor(50, 100, 255, 150));

	guiConfigVirtualJoycons.setup();
	guiConfigVirtualJoycons.setName("ConfigVirtualJoycons");
	guiConfigVirtualJoycons.add(addVirtualJoycon.setup("addVirtualJoycon", guiWidth, guiLineHeight));
	guiConfigVirtualJoycons.add(removeAVirtualJoycon.setup("removeAVirtualJoycon", guiWidth, guiLineHeight));
	guiConfigVirtualJoycons.add(removeAllVirtual.setup("removeAllVirtual", guiWidth, guiLineHeight));
	guiConfigVirtualJoycons.add(virtualJoyconsAddedLabel.setup("numVirtualJoycons", virtualJoyconsAddedLabel, guiWidth, guiLineHeight));
	virtualJoyconsAddedLabel.operator=(ofToString(numVirtualJoycons));
	guiConfigVirtualJoycons.setBackgroundColor(guiColor);
	addVirtualJoycon.setBackgroundColor(guiColor);
	removeAVirtualJoycon.setBackgroundColor(guiColor);
	removeAllVirtual.setBackgroundColor(guiColor);
	virtualJoyconsAddedLabel.setBackgroundColor(guiColor);
	guiConfigVirtualJoycons.setPosition(guiControl.getPosition().x - guiWidth - border, border);

	if (numDevicesConnected == 0) {
		showShortcutsHelp = true;
	}

	winWidthProportion = 4;
	winHeightProportion = 3;
	hMargin = screenWidth / winWidthProportion;
	vMargin = screenHeight / winHeightProportion;
	joyconConfigWindowSettings.setSize(screenWidth - hMargin, screenHeight - vMargin);
	joyconConfigWindowSettings.setPosition(ofVec2f(hMargin / 2, vMargin / 2));
	generalConfigWindowSettings.setSize(screenWidth - hMargin, screenHeight - vMargin);
	generalConfigWindowSettings.setPosition(ofVec2f(hMargin / 2, vMargin / 2));

	oscSender.setup(oscSendNetAddress, oscSendPort);
}

//--------------------------------------------------------------
void ofApp::update(){
	checkAllButtonStates();

	if (joyconsVec.size() > 0) {
		numSelectedJoycons = 0;
		for (int index = 0; index < joyconsVec.size(); index++) {
			if (joyconsVec[index].GUIToggle && (!joyconsVec[index].isVirtual || (joyconsVec[index].isVirtual && useVirtualJoycons)))
				numSelectedJoycons++;
		}

		if (numSelectedJoycons > 0) {
			int joyconCelLines = 1;
			int joyconCelColumns = numSelectedJoycons;

			if (numSelectedJoycons > 2) {
				joyconCelColumns = (numSelectedJoycons / 2) + (numSelectedJoycons % 2);
				joyconCelLines = 2;
			}

			joyconCelsWidth = winWidth / joyconCelColumns;
			joyconCelsHeight = winHeight / joyconCelLines;

			int curColumn = 0;
			int curLine = 0;
			for(int index = 0; index < joyconsVec.size(); index++){
				if (joyconsVec[index].GUIToggle && (!joyconsVec[index].isVirtual || (joyconsVec[index].isVirtual && useVirtualJoycons))) {
					if (curColumn == joyconCelColumns) {
						curColumn = 0;
						curLine++;
					}
					joyconsVec[index].celPosX = curColumn * joyconCelsWidth;
					joyconsVec[index].celPosY = curLine * joyconCelsHeight;
					joyconsVec[index].updateGraphsVisualizations();
					curColumn++;
				}
			}
		}
		else {
			joyconCelsWidth = 0;
			joyconCelsHeight = 0;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(10);
	for each (Joycon joycon in joyconsVec) {
		if (joycon.GUIToggle && (!joycon.isVirtual || (joycon.isVirtual && useVirtualJoycons))) {
			joycon.drawJoycon(joyconCelsWidth, joyconCelsHeight, border);
		}
	}

	if (showGuiControl)
		guiControl.draw();

	if (showGuiJoyconsList)
		guiJoyconsList.draw(); 

	if (showShortcutsHelp && showGuiControl) {
		guiShortcuts.draw();
		guiGraphHelp.draw();
	}

	if (useVirtualJoycons && showGuiControl)
		guiConfigVirtualJoycons.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case 'a':
			if (useVirtualJoycons) {
				executeAddVirtualJoycon = true;
				if (joyconsVec.size() == 0)
					showShortcutsHelp = false;
			}
		break;
		case 'c':
			openGeneralConfigWindow();
			break;
		case 'd':
			executeDisconnectAndDispose = true;
			break;
		case 'g':
			showGuiControl = !showGuiControl;
			break;
		case 'j':
			showGuiJoyconsList = !showGuiJoyconsList;
			break;
		case 'l':
			showGuiJoyconsList = !showGuiJoyconsList;
			break;
		case 'h':
			showShortcutsHelp = !showShortcutsHelp;
			break;
		case 's':
			showShortcutsHelp = !showShortcutsHelp;
			break;
		case 'v':
			useVirtualJoycons = !useVirtualJoycons;
			break;
		case 'r':
			executeRemoveAVirtualJoycon = true;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	winWidth = w;
	winHeight = h;

	guiControl.setPosition(w - guiWidth - border, border);
	guiConfigVirtualJoycons.setPosition(guiControl.getPosition().x - guiWidth - border, border);
	guiJoyconsList.setPosition(border, border);
	guiShortcuts.setPosition((winWidth / 2) - guiWidth, (winHeight / 2) - (guiShortcuts.getHeight() / 2));
	guiGraphHelp.setPosition(guiShortcuts.getPosition().x + guiWidth + border, (winHeight / 2) - (guiGraphHelp.getHeight() / 2));
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::exit() {
	disconnectAndDisposeAll();
}

void ofApp::checkAllButtonStates() {
	if (setUpdateConnected) {
		instantiateConnectedJoycons();
		setupGuiControl();
		setupGuiJoyconsList();
		cout << "Update ended. " << numDevicesConnected << " devices connected." << endl;
		if (numDevicesConnected > 0) {
			cout << "Device ids go from " << (numDevicesConnectedSum - numDevicesConnected) << " to ";
			cout << (numDevicesConnectedSum - numDevicesConnected) + (numDevicesConnected - 1) << "." << endl;
		}
		setUpdateConnected = false;
		framesWaited = 0;
	}

	if (framesWaited == framesToWait) {
		if (updateConnected || executeUpdateConnected) {
			cout << "Researching for connected joycons..." << endl;
			setGuiWithMessage(guiControl, "Reconnecting joycons...");
			setUpdateConnected = true;
			executeUpdateConnected = false;
			showGuiJoyconsList = false;
			guiJoyconsList.clear();
		}
		else if (disconnectAndDispose || executeDisconnectAndDispose) {
			framesWaited = 0;
			disconnectAndDisposeAll();
			setupGuiJoyconsList();
			executeDisconnectAndDispose = false;
		}
		else if (useVirtualJoycons != lastUseVirtualJoyconsValue) {
			framesWaited = 0;
			setupGuiJoyconsList();
			lastUseVirtualJoyconsValue = useVirtualJoycons;
		}
		else if (showShortcutsHelp != lastShowShortcutsHelpValue) {
			framesWaited = 0;
			lastShowShortcutsHelpValue = showShortcutsHelp;
		}
		else if (addVirtualJoycon || executeAddVirtualJoycon) {
			framesWaited = 0;
			Joycon newJoycon(-1, joyconsVec.size(), guiColor.a, font);
			joyconsVec.push_back(newJoycon);
			numVirtualJoycons++;
			setupGuiJoyconsList();
			virtualJoyconsAddedLabel.operator=(ofToString(numVirtualJoycons));
			executeAddVirtualJoycon = false;
		}
		else if (removeAVirtualJoycon || executeRemoveAVirtualJoycon) {
			framesWaited = 0;
			if (numVirtualJoycons > 0) {
				for (int index = joyconsVec.size() - 1; index >= 0; index--) {
					if (joyconsVec[index].isVirtual) {
						numVirtualJoycons--;
						joyconsVec.erase(joyconsVec.begin() + index);
						break;
					}
				}
				setupGuiJoyconsList();
				virtualJoyconsAddedLabel.operator=(ofToString(numVirtualJoycons));
			}
			executeRemoveAVirtualJoycon = false;
		}
		else if (removeAllVirtual) {
			framesWaited = 0;
			if (numVirtualJoycons > 0) {
				for (int index = joyconsVec.size() - 1; index >= 0; index--) {
					if (joyconsVec[index].isVirtual) {
						numVirtualJoycons--;
						joyconsVec.erase(joyconsVec.begin() + index);
					}
				}
				setupGuiJoyconsList();
				virtualJoyconsAddedLabel.operator=(ofToString(numVirtualJoycons));
			}
		}
	}
	else if (framesWaited < framesToWait)
		framesWaited++;
}

void ofApp::instantiateConnectedJoycons() {
	vector<Joycon> virtualJoycons;
	disconnectAndDisposeAll();
	virtualJoycons = joyconsVec;
	joyconsVec.clear();

	numDevicesConnected = JslConnectDevices(); //gets the ammount of currently connected devices
	connectedDevicesLabel.operator=(ofToString(numDevicesConnected));

	if (numDevicesConnected > 0) {
		joyconCelsWidth = winWidth / numDevicesConnected;
		joyconCelsHeight = winHeight / numDevicesConnected;

		int lastDeviceId = numDevicesConnected + numDevicesConnectedSum - 1;
		int devicePosition = 0; //the position of the device on 'joyconsVec'
		for (int deviceId = numDevicesConnectedSum; deviceId <= lastDeviceId; deviceId++, devicePosition++) {
			Joycon newJoycon(deviceId, devicePosition, guiColor.a, font);
			joyconsVec.push_back(newJoycon);
		}
	}
	numDevicesConnectedSum = numDevicesConnected + numDevicesConnectedSum;

	JslSetCallback(&getJslCallback); //_n2

	if (virtualJoycons.size() > 0) { //_n3
		for (int index = 0; index < virtualJoycons.size(); index++) {
			joyconsVec.push_back(virtualJoycons[index]);
		}
	}
}

void ofApp::disconnectAndDisposeAll() {
	if (numDevicesConnected > 0) {
		JslDisconnectAndDisposeAll();
		for (int index = 0; index < joyconsVec.size(); index++) {
			if (!joyconsVec[index].isVirtual) {
				joyconsVec.erase(joyconsVec.begin() + index);
				index--;
			}
		}
		numDevicesConnected = 0;
		connectedDevicesLabel.operator=(ofToString(numDevicesConnected));
	}
}

void ofApp::updateJoyconData(int joyconId, JOY_SHOCK_STATE newButtonsStickData, IMU_STATE newRawIMUData) {
	int firstPos = (numDevicesConnectedSum - numDevicesConnected);
	int joyconPosVec = joyconId - firstPos;
	inputValues currentInputValues = getEachInputValue(newButtonsStickData, joyconsVec[joyconPosVec].controllerType);
	joyconsVec[joyconPosVec].rawIMUData = newRawIMUData;
	joyconsVec[joyconPosVec].cookedIMUData = JslGetMotionState(joyconId);
	sendNewInputsAsOSC(joyconsVec[joyconPosVec].currentInputValues, currentInputValues, joyconsVec[joyconPosVec]);
	joyconsVec[joyconPosVec].currentInputValues = currentInputValues;
}

inputValues ofApp::getEachInputValue(JOY_SHOCK_STATE newButtonsStickData, int controllerType){//_n4
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

void ofApp::sendNewInputsAsOSC(inputValues lastButtonValues, inputValues currentInputValues, Joycon updatedJoycon) {
	if (currentInputValues.upX != lastButtonValues.upX)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/upX", currentInputValues.upX));
	if(currentInputValues.downB != lastButtonValues.downB)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/downB", currentInputValues.downB));
	if(currentInputValues.leftY != lastButtonValues.leftY)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/leftY", currentInputValues.leftY));
	if(currentInputValues.rightA != lastButtonValues.rightA)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/rightA", currentInputValues.rightA));
	if(currentInputValues.minusPlus != lastButtonValues.minusPlus)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/minusPlus", currentInputValues.minusPlus));
	if(currentInputValues.stickClick != lastButtonValues.stickClick)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/stickClick", currentInputValues.stickClick));
	if(currentInputValues.lr != lastButtonValues.lr)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/lr", currentInputValues.lr));
	if(currentInputValues.zlzr != lastButtonValues.zlzr)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/zlzr", currentInputValues.zlzr));
	if(currentInputValues.printHome != lastButtonValues.printHome)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/printHome", currentInputValues.printHome));
	if(currentInputValues.sl != lastButtonValues.sl)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/sl", currentInputValues.sl));
	if(currentInputValues.sr != lastButtonValues.sr)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/sr", currentInputValues.sr));
	if (abs(currentInputValues.stickX - lastButtonValues.stickX) >= minStickStep) 
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/stickX", currentInputValues.stickX));
	if (abs(currentInputValues.stickY - lastButtonValues.stickY) >= minStickStep)
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/stickY", currentInputValues.stickY));

	if (updatedJoycon.useRawIMUData) {
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/gyroX", updatedJoycon.rawIMUData.gyroX));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/gyroY", updatedJoycon.rawIMUData.gyroY));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/gyroZ", updatedJoycon.rawIMUData.gyroZ));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/raclX", updatedJoycon.rawIMUData.accelX));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/raclY", updatedJoycon.rawIMUData.accelY));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/raclZ", updatedJoycon.rawIMUData.accelZ));
	}
	if (updatedJoycon.useCookedIMUData) {
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/quatW", updatedJoycon.cookedIMUData.quatW));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/quatX", updatedJoycon.cookedIMUData.quatX));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/quatY", updatedJoycon.cookedIMUData.quatY));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/quatZ", updatedJoycon.cookedIMUData.quatZ));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/caclX", updatedJoycon.cookedIMUData.accelX));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/caclY", updatedJoycon.cookedIMUData.accelY));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/caclZ", updatedJoycon.cookedIMUData.accelZ));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/gravX", updatedJoycon.cookedIMUData.gravX));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/gravY", updatedJoycon.cookedIMUData.gravY));
		oscSender.sendMessage(getInputOscMessage(updatedJoycon.oscAddress, "/gravZ", updatedJoycon.cookedIMUData.gravZ));
	}
}

ofxOscMessage ofApp::getInputOscMessage(string oscAddress, string inputAddress, float inputValue) {
	ofxOscMessage inputMessage;
	inputMessage.setAddress(oscAddress + inputAddress);
	inputMessage.addFloatArg(inputValue);
	return inputMessage;
};

void ofApp::setupGuiControl() {
	guiControl.setup();
	guiControl.setName("GUIControl");
	guiControl.add(updateConnected.setup("updateConnected", guiWidth, guiLineHeight));
	guiControl.add(disconnectAndDispose.setup("disconnect&DisposeAll", guiWidth, guiLineHeight));
	guiControl.add(useVirtualJoycons.setup("useVirtualJoycons", false, guiWidth, guiLineHeight));
	guiControl.add(connectedDevicesLabel.setup("numConnectedDevices", connectedDevicesLabel, guiWidth, guiLineHeight));
	guiControl.add(showShortcutsHelp.setup("shortcuts/help", false, guiWidth, guiLineHeight));
	connectedDevicesLabel.operator=(ofToString(numDevicesConnected));
	guiControl.setPosition(winWidth - guiWidth - border, border);

	guiControl.setBackgroundColor(guiColor);
	updateConnected.setBackgroundColor(guiColor);
	disconnectAndDispose.setBackgroundColor(guiColor);
	connectedDevicesLabel.setBackgroundColor(guiColor);
	useVirtualJoycons.setBackgroundColor(guiColor);
	showShortcutsHelp.setBackgroundColor(guiColor);
}

void ofApp::setupGuiJoyconsList(){
	guiJoyconsList.setup();
	guiJoyconsList.setName("JoyconsList");
	guiJoyconsList.setPosition(border, border);
	guiJoyconsList.setBackgroundColor(guiColor);

	if(joyconsVec.size() > 0){
		for(int index = 0; index < joyconsVec.size(); index++){
			if (!joyconsVec[index].isVirtual || (joyconsVec[index].isVirtual && useVirtualJoycons)) {
				string joyconGUILabel = joyconsVec[index].nameOnGUI;
				guiJoyconsList.add(joyconsVec[index].GUIToggle.setup(joyconGUILabel, true, guiWidth, guiLineHeight));
				joyconsVec[index].GUIToggle.setBackgroundColor(joyconsVec[index].joyconColor);
			}
		}
		showGuiJoyconsList = true;
	}
	else {
		showGuiJoyconsList = false;
	}

	numSelectedJoycons = numDevicesConnected;
}

void ofApp::setGuiWithMessage(ofxPanel& gui, string message) {
	gui.clear();
	gui.setBackgroundColor(guiColor);
	gui.add(messageLabel.setup(message, messageLabel, guiWidth, guiLineHeight));
}

void ofApp::openGeneralConfigWindow() {
	generalConfigWindowSettings.title = "General Config";
	generalConfigWindow = ofCreateWindow(generalConfigWindowSettings);
	ofAddListener(generalConfigWindow->events().draw, this, &ofApp::drawGeneralConfigWindow);
}

void ofApp::drawGeneralConfigWindow(ofEventArgs &args) {
}

void ofApp::openJoyconConfigWindow(Joycon& joyconToConfig) {
	joyconConfigWindowSettings.title = "Config: " + joyconToConfig.nameOnGUI;
	joyconConfigWindow = ofCreateWindow(joyconConfigWindowSettings);
	ofAddListener(joyconConfigWindow->events().draw, this, &ofApp::drawJoyconConfigWindow);
}

void ofApp::drawJoyconConfigWindow(ofEventArgs &args) {

}

/*
	n1- sets the local pointer 'joyOSCMapperPointer' with the adress of the new ofApp() created on main.cpp. 
		This is used with the 'getJslCallback' function, that receives the data from all joycons and updates them;
	n2- sets a callback function by wich JoyShockLibrary reports the data from all connected devices at their
		proper rate (for joycons, 66.67hz). This callback function needs to be reset every time after calling 
		'JslDisconnectAndDisposeAll()';
	n3- puts the virtual joycons always on the end of joyconsVec. The ideia here is to open an easy access from the 
		callback update function to the connected 'real' joycons, not needing any 'ifs' or 'fors' to find the 
		correct joycon to update;
	n4- the stats/values of all joycon buttons come in a single integer number, with each bit of the integer
		corresponding to a button. The correct bits for each button are here JSMASK constants, defined
		on JoyShockLibrary. If a button is pressed, the value should be 1, if not, 0;
*/