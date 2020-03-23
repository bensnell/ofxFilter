#include "ofxFilterOpContinuity.h"

// --------------------------------------------------
void ofxFilterOpContinuitySettings::setupParams() {

	RUI_SHARE_PARAM_WCN(getID() + "- Friction", friction, 0, 1);



}

// --------------------------------------------------
void ofxFilterOpContinuity::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

	// The data we predict from is initially invalid (until we see valid data)
	//outData.bValid = false;

}

// --------------------------------------------------
void ofxFilterOpContinuity::process(ofxFilterData& data) {
	if (data.r.size() != 3) {
		ofLogError("ofxFilterOpContinuity") << "Requires exactly 3rd order rate (motion) params";
	}

	ofLogNotice("ofxFilter") << "Processing continuity";

	ofxFilterOpContinuitySettings* s = static_cast<ofxFilterOpContinuitySettings*>(settings);

	// Rate should be calculated in the previous layer ("add-rate")


	if (bLastDataValid && !data.bValid) {

		if (obsAmt == 1.0) {
			obsAmt = 0.0; // fully predicted data
		}
		else {
		
			// ? subtract 0.1 ?
		
		}
	}
	bLastDataValid = data.bValid;


	// If the data is valid and the output is fully observed data, then set the outData
	// (this should happen at the beginning of every sequence)
	if (data.bValid && obsAmt == 1.0) {
		outData = data;
	}
	else {

		// Begin with the last output params
		tmpData = outData;

		// TODO (?): Only fill holes that are large enough?

		// If the data is invalid...
		// Or if the mix is not fully observed...
		// Then some prediction must be made.
		
		// TODO (?): If there is valid data, should the past motion params be interpolated?

		// outData contains the most recent motion (rate) parameters

		// First, apply friction to the parameters (one frame)
		tmpData.applyFriction(s->friction); // TODO: don't apply friction if there is valid data
		// Update the rate params (one frame)
		tmpData.predictFromRate();
		// Then, use outData to predict the current transformation matrix
		tmpData.setFromRate();

		if (data.bValid) {

			// Update the mixture ratio betwen observed and predicted data
			obsAmt = CLAMP(obsAmt + 0.001, 0, 1);

			// Interpolate this prediction to the actual one, accounting for the anticipated speed
			tmpData.lerp(data, 0.01);

			//outData = tmpData; // DOES NOT WORK 

			// Correct the predData using this new measurement
			outData.set(tmpData.getTranslation(), tmpData.getRotation(), tmpData.getScale());
			// Regenerate motion parameters using this measurement ????
			outData.updateRate();

			// TODO: Should an acceleration be applied instead of lerping?

		}
		else {

			// Last data was invalid, so continue using the last motion params
			outData = tmpData;

		}

	}


	// TODO: should data always be changed?
	// TODO: should it always be valid?

	outData.bValid = true;
	data = outData;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
