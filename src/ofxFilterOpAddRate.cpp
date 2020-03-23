#include "ofxFilterOpAddRate.h"

// --------------------------------------------------
void ofxFilterOpAddRateSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getID() + "- Order", order, 1, 4);
	RUI_SHARE_PARAM_WCN(getID() + "- Frames to Reset", nFramesToResetRate, 0, 120);

}

// --------------------------------------------------
void ofxFilterOpAddRate::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpAddRate::process(ofxFilterData& data) {

	ofxFilterOpAddRateSettings* s = static_cast<ofxFilterOpAddRateSettings*>(settings);

	// Initialize the rate parameters if the order is not the same
	if (data.r.size() != s->order) {
		data.r.init(s->order);
	}
	else if (data.bValid && !lastData.bValid && (s->nFramesToResetRate > nFramesSinceValidData)) {
		data.r.init(s->order);
	}

	// Update the rate parameters if available (using the most recent data)
	if (data.bValid) {
	
		if (nFramesSinceValidData < 0) nFramesSinceValidData = 1;

		data.updateRate(nFramesSinceValidData);
		nFramesSinceValidData = 0;
	}

	if (nFramesSinceValidData >= 0) nFramesSinceValidData++;
	lastData = data;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
