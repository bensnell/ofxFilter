#include "ofxFilterOpAddRate.h"

// --------------------------------------------------
void ofxFilterOpAddRateSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getID() + "- Order", order, 1, 4);
	RUI_SHARE_PARAM_WCN(getID() + "- Reset After Empty", bResetRateAfterEmptyData);

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
	else if (data.bValid && !lastData.bValid && s->bResetRateAfterEmptyData) {
		data.r.init(s->order);
	}

	// Update the rate parameters if available (using the most recent data)
	if (data.bValid) {
	
		if (nFramesSinceLastValidData < 0) nFramesSinceLastValidData = 1;

		data.updateRate(nFramesSinceLastValidData);
		nFramesSinceLastValidData = 0;
	}

	if (nFramesSinceLastValidData >= 0) nFramesSinceLastValidData++;
	lastData = data;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
