#include "ofxFilterOpEasing.h"

// --------------------------------------------------
void ofxFilterOpEasingSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getID() + "- Easing Param", easingParam, 0, 1);

}

// --------------------------------------------------
void ofxFilterOpEasing::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpEasing::process(ofxFilterData& data) {
	if (!data.bValid) return;

	// Skip the first process
	if (bFirstEase) {
		bFirstEase = false;
		lastValidData = data;
		return;
	}

	float easingParam = static_cast<ofxFilterOpEasingSettings*>(settings)->easingParam;

	// Process the data
	// Apply an ease by lerping data
	data.lerp(lastValidData, easingParam);

	// Save the last data
	lastValidData = data;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
