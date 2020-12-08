#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"

class ofxFilterOpAxesSettings : public ofxFilterOpSettings {
public:

	ofxFilterOpAxesSettings() {
		type = "axes";
		abbr = "AXS";
	};
	~ofxFilterOpAxesSettings() {};

	void setupParams();

	// Coordinate system handedness
	enum HandednessMode {
		RIGHT_HANDED = 0,
		LEFT_HANDED
	};
	static vector<string> getHandednessModes() { return { "Right", "Left" }; };
	HandednessMode srcHandedness = RIGHT_HANDED;	// must provide

	// Coordinate system up vector
	enum UpVectorMode {
		UP_X = 0,
		UP_Y,
		UP_Z
	};
	static vector<string> getUpVectorModes() { return { "X", "Y", "Z" }; };
	UpVectorMode srcUpVector = UP_Y;		// must provide

	// Convert handedness?
	bool bConvertHandedness = true;
	HandednessMode dstHandedness = RIGHT_HANDED;

	// Convert up vector?
	bool bConvertUpVector = true;
	UpVectorMode dstUpVector = UP_Y;

	// Number of (90 degree) rotations around the up vector.
	// (# clockwise rotations)
	int nUpVectorRotations = 0;
};

class ofxFilterOpAxes : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);
};
