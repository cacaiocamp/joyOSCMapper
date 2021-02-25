#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "Joycon.h"
#include "sharedDefs.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void exit();

		void checkAllButtonStates();
		void instantiateConnectedJoycons();
		void disconnectAndDisposeAll();
		void updateJoyconData(int deviceId, JOY_SHOCK_STATE newButtonsStickData, IMU_STATE newRawIMUData);
		void updateJoyconsDrawings();
		void setupGuiJoyconsList();
		void setupGuiControl();
		void setGuiWithMessage(ofxPanel& gui, string message);

		void openGeneralConfigWindow();
		void drawGeneralConfigWindow(ofEventArgs &args);
		void mousePressedGeneralConfigWindow(ofMouseEventArgs &args);

		void openJoyconConfigWindow(Joycon& joyconToConfig);
		void drawJoyconConfigWindow(ofEventArgs &args);

		int winWidth = 0; int winHeight = 0;

		int numDevicesConnected = 0;
		int numDevicesConnectedSum = 0; //_n1
		int numVirtualJoycons = 0;
		vector<Joycon> joyconsVec;
		int numSelectedJoycons = 0;
		float joyconCelsWidth = 0;
		float joyconCelsHeight = 0;
		int joyconCelPressedIndex = -1;
		string clickedButtonOscTag = "";
		string clickedButtonOscMessage = "";
		ofVec2f clickedButtonPos;
		ofTrueTypeFont font;
		stringstream oscOnlyInfo;
		ofxOscReceiver oscReceiver;
		int oscReceiverPort = DEFAULT_OSCRECEIVE_PORT;

		//guiControl variables
		ofxPanel guiControl;
		bool showGuiControl = true;
		ofxButton updateConnected;
		bool executeUpdateConnected = false;
		bool setUpdateConnected = false; //_n2
		ofxButton disconnectAndDispose;
		bool executeDisconnectAndDispose = false;
		ofxToggle useVirtualJoycons;
		bool lastUseVirtualJoyconsValue = false;
		ofxLabel connectedDevicesLabel;
		ofxToggle useEulerOrientation;
		bool lastUseEulerOrientationValue = false;
		ofxToggle calibrateJoycons;
		bool lastCalibrateJoyconsValue = false;
		ofxToggle oscOnly;
		bool lastOscOnlyValue = false;
		ofxToggle showShortcutsHelp;
		bool lastShowShortcutsHelpValue = false;

		//guiJoyconsList variables
		ofxPanel guiJoyconsList;
		bool showGuiJoyconsList = false;

		//guiConfigVirtualJoycons variables
		ofxPanel guiConfigVirtualJoycons;
		ofxButton addVirtualJoycon;
		bool executeAddVirtualJoycon = false;
		ofxButton removeAVirtualJoycon;
		bool executeRemoveAVirtualJoycon = false;
		ofxButton removeAllVirtual;
		ofxLabel virtualJoyconsAddedLabel;

		//guiShortcuts variables
		ofxPanel guiShortcuts;
		ofxLabel sShortcut;
		ofxLabel cShortcut;
		ofxLabel dShortcut;
		ofxLabel gShortcut;
		ofxLabel jShortcut;
		ofxLabel uShortcut;
		ofxLabel vShortcut;
		ofxLabel aShortcut;
		ofxLabel rShortcut;
		ofxLabel eShortcut;
		ofxLabel oShortcut;
		ofxLabel leftClickHelp;
		ofxLabel rightClickHelp;
		bool showGuiShortcuts = false;

		//guiGraphHelp
		ofxPanel guiGraphHelp;
		ofxLabel xiColorHelp;
		ofxLabel yjColorHelp;
		ofxLabel zkColorHelp;
		ofxLabel wColorHelp;

		//GUI variables
		int guiWidth = 200;
		int guiLineHeight = 20;
		int framesToWait = 5; //number of frames to wait between consecutive clicks
		int framesWaited = framesToWait; //number of frames waited
		ofxLabel messageLabel;
		ofColor guiColor = ofColor(0, 0, 0, 120);

		//other windows
		ofGLFWWindowSettings generalConfigWindowSettings;
		shared_ptr<ofAppBaseWindow> generalConfigWindow;
		int generalConfigWindowWidth; int generalConfigWindowHeight;
		ofGLFWWindowSettings joyconConfigWindowSettings;
		shared_ptr<ofAppBaseWindow> joyconConfigWindow;
};

/*
	n1- JoyShockLibrary continues counting up the devices when you desconnect and reconnect them. As this 
		happens with the 'updateConnected' GUI button, those 2 variables are used to control the deviceIds.
		Therefore, 'numDevicesConnectedSum' is used as the first deviceId in 'ofApp::instatiateJoycons()',
		with 'numConnectedDevices' being the number of real joycons connected;
	n2- This boolean is used with 'updateConnected' button. The button sets a message to the GUI informing
		that the update is running and sets the boolean to true, drawing the message on GUI on the current
		frame and starting the update on the next one (cause the boolean is true). This is made just to allow
		the user to see the reconnecting message, as the 'JslConnectDevices()' may take a while to process
		and stops all other functions when its running in the way it has been implemented here;
*/