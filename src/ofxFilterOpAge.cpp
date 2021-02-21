#include "ofxFilterOpAge.h"

// --------------------------------------------------
void ofxFilterOpAgeSettings::setupParams() {

	// Don't create a new group, just add on params
    RUI_SHARE_PARAM_WCN(getIDA() + "- Min Age", minAge, 0, 10000);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Consecutive", consecutive);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Other Ops Extend Validity", otherOpsExtendValidity);

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

		// If the data is invalid and we're looking for consecutive data,
		// then reset the age.
		if (s->consecutive && !data.bValid) age = 0;

		// If the data is valid, then increment the age
		if (data.bValid) age++;

		// Update whether this was valid
		bLastProcess0Valid = data.bValid;

	}; break;
	case 1: {

		// If the data is valid, but isn't old enough, consider its future validity
		if (data.bValid && age < s->minAge) {

			// If this data was valid last frame, and if we're allowing
			// ops to extend validity, then keep it valid.
			// (It shouldn't matter whether the last process was valid).
			if (bLastProcess1Valid && s->otherOpsExtendValidity)
			{
				// Allow data to remain valid
			} else
			{
				// Invalidate the data
				data.bValid = false;
			}
		}
			
		// Update whether the data was previously valid
		bLastProcess1Valid = data.bValid;

	}; break;
	}
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
