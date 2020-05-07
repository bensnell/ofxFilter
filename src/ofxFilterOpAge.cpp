#include "ofxFilterOpAge.h"

// --------------------------------------------------
void ofxFilterOpAgeSettings::setupParams() {

	// Don't create a new group, just add on params
    RUI_SHARE_PARAM_WCN(getIDA() + "- Min Age", minAge, 0, 10000);

}

// --------------------------------------------------
void ofxFilterOpAge::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpAge::_process(ofxFilterData& data) {

	ofxFilterOpAgeSettings* s = static_cast<ofxFilterOpAgeSettings*>(settings);

	// This operator should be called twice
	switch (getProcessCount()) {
	case 0: {

		// The first time this is called, check if there is valid data
		bThereWasValidData |= data.bValid;

		// If there has ever been valid data, increment the age
		if (bThereWasValidData) age++;

	}; break;
	case 1: {

		// If the current age does not exceed threshold, the invalidate the data
		if (data.bValid && age < s->minAge) data.bValid = false;

	}; break;
	}
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
