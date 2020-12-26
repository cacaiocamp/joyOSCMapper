# joyOSCMapper
**joyOSCMapper** is an easy way to visualize and map your joycon inputs (buttons, stick and motion) as [OSC](http://opensoundcontrol.org/introduction-osc) messages. It is built with C++/[openFrameworks](https://openframeworks.cc/) and uses JibbSmarts' [JoyShockLibrary](https://github.com/JibbSmart/JoyShockLibrary) to gather the data from the joycons via Bluetooth and the *ofxOsc* addon to convert the data in OSC messages. This software is being built as a simple program to use joycons with Interactive Music Systems and other art related contexts. 

I've developted it using a Windows 10, with Visual Studio 2017, but, as its built with openFrameworks, it should be easy enough to use it in all the OSs that run this C++ toolkit.

## Instalation
With the openFrameworks already downloaded for your IDE, you will first need build a .dll version of JoyShockLibrary for your OS. If you are using a x64 Windows, the .dll and .lib files in the _libs/_ folder are already the ones. If x86, you can get the [latest x86 release of JoyShockLibrary](https://github.com/JibbSmart/JoyShockLibrary/releases).

After that, all the files you need are on the _src/_ folder. You will also need to make the link of your project/solution to the folder where the .dll and .lib files are and a openFrameworks app with _ofxOsc_ and _ofxGUI_ addons imported.

For Visual Studio users, the full solution is also provided. I will also try to make a standalone app in the future.

## OSC data sent
**joyOSCMapper** takes advantage of most of JoyShockLibrary's functionalities for joycons, mapping buttons, sticks, raw motion (gyroscope and accelerometer) and sensor fusion (orientation in quaternions, acceleroremeter without the effect of gravity and isolated gravity). I also implemented the stick data as polar coordinates and as dpad. This data is collected, drawn in real time and sent via OSC. 

The full list of the default input OSC tags follows (the name of the buttons are composite to represent both left and right joycons): 
```
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
	string caclZ = "/caclZ"; //"cooked" acceleration, without gravity
	string gravX = "/gravX";
	string gravY = "/gravY";
	string gravZ = "/gravZ";
};
```

The full OSC address is composed with the name of the joycon (joycon+0..n, based on the order of connection) added with the osc tag of the input pressed, followed by the argument with the value of the input. So, for example, if I press the up/X button, with only one joycon connected, the full OSC message sent would be ```/joycon0/upX 1.0```.

With the exception of the motion data, all OSC messages are sent only when a change on the input value occurs.

## Basic functionalities
### Connecting joycons
You only need to connect the joycons over bluetooth to your computer than execute **joyOSCMapper** and the program should manage to connect to them and will automaticaly start to send their respective OSC messages. You can also use two buttons on GUIControl, ```updateConnected``` and ```disconnect&DisposeAll```, to manipulate joycon connections.

### oscOnly mode
When toggled, the oscOnly mode will stop all joycon drawing but will continue to send the OSC messages for the inputs made. You can either un/toggle them by clicking the toggle ```oscOnly``` on GUIControl or sending an OSC message to the address ```/oscOnly```, with 0 or 1 as argument.

### JoyconsList
JoyconsList is a _ofxPanel_ GUI that has a toggle for every connected joycon. You can un/toggle the drawing and the OSC messages of each one.

### Graphs
The graphs show a collection of the last second of each IMU data collected, in all axes. The color pattern for each axis is: 
* x = red
* y = green
* z = blue
* w = white (used on the quaternion orientation)
