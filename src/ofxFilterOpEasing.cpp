#include "ofxFilterOpEasing.h"

// --------------------------------------------------
void ofxFilterOpEasingSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getIDA() + "- Easing Param", easingParam, 0, 1);
    RUI_SHARE_PARAM_WCN(getIDA() + "- Frames To Reset", nEmptyFramesToReset, 0, 240);

}

// --------------------------------------------------
void ofxFilterOpEasing::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpEasing::_process(ofxFilterData& data) {

    // Increment the number of frames
    nFramesSinceObs++;

    // If we don't have valid data, return
    if (!data.bValid) return;
    
    // Get the settings for this operator
    ofxFilterOpEasingSettings* s = static_cast<ofxFilterOpEasingSettings*>(settings);

    if (bFirstEase || nFramesSinceObs > s->nEmptyFramesToReset) {
        // Skip the first process or reset the ease
        bFirstEase = false;

    } else {
        // Process the data
        // Apply an ease by lerping data
        data.lerp(lastValidData,
                  ofxFilterUnits::one()->convertEaseParam(s->easingParam, 60));
    }

    // Save the last data
    lastValidData = data;

    // This is valid data, so reset the elapsed frames
    nFramesSinceObs = 0;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
