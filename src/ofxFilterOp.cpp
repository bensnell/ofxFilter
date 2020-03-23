#include "ofxFilterOp.h"

void ofxFilterOpSettings::setupParams() {
	// Don't create a new group, just add on params
	//RUI_SHARE_PARAM_WCN(getID()+"- Param Name", xxx)
}

// --------------------------------------------------
void ofxFilterOp::setup(ofxFilterOpSettings* _settings) {

	// Update the settings
	settings = _settings;

}

// --------------------------------------------------
void ofxFilterOp::process(ofxFilterData& data) {
	
	// Empty processes should pass through data unless an operator is explicitly
	// designed to process empty data
	if (!data.bValid) return;

	// Process the input
	data = data;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
