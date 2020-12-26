# joyOSCMapper
**joyOSCMapper** is suposed to be a easy way to visualize and map your joycon inputs (buttons, stick and motion) as [OSC](http://opensoundcontrol.org/introduction-osc) messages, allowing to use and test joycon inputs to control "anything" controlable with OSC. Therefore, this software idea is a simple central program to use joycons with Interactive Music Systems and other art related contexts. 

It is being built with C++/[openFrameworks](https://openframeworks.cc/) and uses JibbSmarts' [JoyShockLibrary](https://github.com/JibbSmart/JoyShockLibrary) to gather the data from the joycons via Bluetooth and the *ofxOsc* addon to convert the data in OSC messages. I'm developting it using a Windows 10, with Visual Studio 2017, but, as its built with openFrameworks, it should be easy enough to use it in every OS that run this C++ toolkit.

## Contents
* **[Instalation](#instalation)**
* **[OSC data sent](#osc-data-sent)**
* **[Basic functionalities](#basic-functionalities)**
  * **[Connecting joycons](#connecting-joycons)**
  * **[oscOnly mode](#oscOnly-mode)**
  * **[JoyconsList](#joyconslist)**
  * **[Motion graphs](#motion-graphs)**
* **[Other functionalities](#other-functionalities)**
  * **[Virtual Joycons](#virtual-joycons)**
  * **[Mouse clicks](#mouse-clicks)**
  * **[Shortcuts/Help](#shortcutshelp)**
* **[Important notes](#important-notes)**
* **[Future implementations](#future-implementations)**
* **[Last words/contact](#last-wordscontact)**
* **[License](#license)**

## Instalation
With the openFrameworks already downloaded for your IDE, you will first need to build a .dll of JoyShockLibrary for your OS. If you are using a x64 Windows, the .dll and .lib files in the _libs/_ folder are already the ones. If x86, you can get the [latest x86 release of JoyShockLibrary](https://github.com/JibbSmart/JoyShockLibrary/releases).

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

The full OSC address is composed with the name of the joycon (/joycon+0..n, based on the order of connection) added with the OSC tag of the input pressed, followed by the argument with the value of the input. So, for example, if I press the up/X button on the first joycon connected the full OSC message sent will be ```/joycon0/upX 1.0```.

With the exception of the motion data, all OSC messages are sent only when a change on the input value occurs. The joycons are updated in a 66.67hz frequency, receiving the latest state for all inputs every 15ms. Therefore, thats the same rate in which **joyOSCMapper** can send OSC messages and the ideal fps for the aplication (rounded up to 67).

## Basic functionalities
### Connecting joycons
You only need to connect the joycons over bluetooth to your computer than execute **joyOSCMapper** and the program should manage to connect to them, automaticaly starting to draw them and to send their respective OSC messages. You can also use two _ofxButton_ on GUIControl, ```updateConnected``` and ```disconnect&DisposeAll```, to manipulate joycon connections, as well as a _ofxLabel_ with the number of connected joycons.

### oscOnly mode
When toggled, the oscOnly mode will stop all joycon drawing but will continue to send the OSC messages for the inputs made. You can either un/toggle them by clicking the toggle ```oscOnly``` on GUIControl or sending an OSC message to the address ```/oscOnly```, with 0 or 1 as argument.

### JoyconsList
JoyconsList is a _ofxPanel_ GUI that has a toggle for every connected joycon. You can un/toggle the drawing and the OSC messages of each one.

### Motion graphs
The graphs show a collection of the last second of each IMU data collected, in all axes. The color pattern for each axis is: 
* x = red
* y = green
* z = blue
* w = white (used on the quaternion orientation)

## Other functionalities
### Virtual joycons
Virtual joycons are fake joycons used to test **joyOSCMapper** and its interaction with other programs even when without a joycon. Their motion data is randomized every frame, and stick and buttons can be manipulated with mouse clicks, sending respective OSC messages when ```useVirtualJoycons``` is toggled on the GUIControl. For example, when you left click at the up/X button of the first added virtual joycon, the OSC message sent will be ```/vrtjoy0/upX 1.0```.

Toggling ```useVirtualJoycons``` will open another _ofxPanel_ GUI, where you can control/manage de addition and removal of virtual joycons, as well as see the number of virtual joycons created. Virtual joycons also have toggles on JoyconsList

### Mouse clicks
Right mouse clicks will show the respective OSC address for the input clicked, as well as the range of values for non boolean inputs. The left clicks will activate joycons buttons and stick on virtual joycons, while doing the same as right clicks on real connected joycons.

### Shortcuts/Help
The shorcuts/help can be un/toggled on the GUIControl. It will show the keyboard shorcuts implemented, as well as the color information for the graphs.

## Important notes
**joyOSCMapper** will allow any number of joycons to be connected (as well as any number of virtual joycons to be created), trying to respond to their inputs on joycons data update rate, **every 15ms/66.67hz**. But, as noted on [this](https://github.com/JibbSmart/JoyShockLibrary#known-and-perceived-issues) part of JoyShockLibrary's README.md: "Some Bluetooth adapters can't keep up with these devices [joycons], resulting in laggy input. This is especially common when more than one device is connected (such as when using a pair of JoyCons). There is nothing JoyShockMapper or JoyShockLibrary can do about this.".

It's also worth to notice that trying to draw too many joycons may result in a fps drop and that may cause more laggy inputs/OSC messages. For those cases (or any other reason to fps drop), the use of _oscOnly mode_ is recomended.

## Future implementations
Not counting the standalone app, I plan to implement two types of configurations: a general config, and a individual config for each joycon. The ideia here is to let drawing and OSC be configured accordingly to user necessities, changing things like:
* un/toggle the use/draw raw and/or cooked graphs
* change osc address, port and tags
* change minStickStep (the minimum value to send stick data OSC messages)
* un/toggle the use stick as D-pad/polar coordinates
* chance stickAsDpadProp (the minimum value to trigger the stickDpad)
* change IMUVectorsSize (the size of the motion graphs)
* save and load preferences

## Last words/contact
This is my first public repository and I might have done something wrong with it, sorry in advance for that! 

I made this program as a way to study openFrameworks and make something that I thought might be usefull (for me and other people). So please, feel free to open issues if you find any bug or want make a sugestion. For any other kind of contact, regarding doubts, comments or if you are using **joyOSCMapper** and wanna hit me up for a chat, also feel free to contact me by [email](https://github.com/cacaiocamp).

Last, I'd also like to give credits to [JoyShockLibrary](https://github.com/JibbSmart/JoyShockLibrary) developers, as this program mainly depends on it to work.

## License
**joyOSCMapper** is licensed under the MIT License - see LICENSE.md.
