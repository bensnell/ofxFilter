#include "ofxFilterOpPersist.h"

// --------------------------------------------------
void ofxFilterOpPersistSettings::setupParams() {

	// Don't create a new group, just add on params
    RUI_SHARE_PARAM_WCN(getIDA() + "- Num Frames", nFrames, 0, 100000);

}

// --------------------------------------------------
void ofxFilterOpPersist::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpPersist::_process(ofxFilterData& data) {

	ofxFilterOpPersistSettings* s = static_cast<ofxFilterOpPersistSettings*>(settings);

	// Incrememnt the number of invalid frames
	nInvalidFrames = data.bValid ? 0 : (nInvalidFrames + 1);

	if (s->nFrames < 0 || (nInvalidFrames <= s->nFrames)) {
		// Force this data to be valid
		data.bValid = true;
	}
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
