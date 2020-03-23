#include "ofxFilterOpContinuity.h"

// --------------------------------------------------
void ofxFilterOpContinuitySettings::setupParams() {

	RUI_SHARE_PARAM_WCN(getID() + "- Rate Order for Export", rateOrderToBeginExport, 0, 3);
	RUI_SHARE_PARAM_WCN(getID() + "- Copy Linked Rate", bCopyLinkedRate);

	RUI_SHARE_PARAM_WCN(getID() + "- Friction", friction, 0, 1);



}

// --------------------------------------------------
void ofxFilterOpContinuity::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

	predData.bValid = false;

}

// --------------------------------------------------
void ofxFilterOpContinuity::process(ofxFilterData& data) {
	if (data.r.size() != 3) {
		ofLogError("ofxFilterOpContinuity") << "Requires exactly 3rd order rate (motion) params. Add the op 'add-rate' prior.";
		return;
	}

	// Get the settings for this operator
	ofxFilterOpContinuitySettings* s = static_cast<ofxFilterOpContinuitySettings*>(settings);

	// If the observed data is valid and the required rate is present,
	// then begin exporting data
	if (data.bValid && data.r.b[min(s->rateOrderToBeginExport, data.r.size())]) {
		bExporting = true;
	}

	// If we're not exporting, mark output data as invalid
	if (!bExporting) {
		data.bValid = false;
		return;
	}

	// We are currently exporting data

	if (bLinked) {

		if (data.bValid) {
			if (!predData.bValid) {
				// Set the predicted data for the very first time
				predData.bValid = true;
				predData = data;
			}
			else {

				// What would the current prediction be if we proceed without observations?
				tmpData = predData;
				tmpData.r.backward();
				tmpData.setFromRate();

				// If many frames have elapsed and the observed data differs significantly from 
				// predicted data, then this can no longer be linked. Proceed to unlinked
				// instructions
				if (nFramesSinceObs > s->nFramesUnlinkThresh && !tmpData.similar(data, 1, 1, 1)) {
					bLinked = false;
				}
				else {
					// Reconcile new (known) data with previous (perhaps uncertain) data.
					predData.reconcile(data, s->linkReconMode);
				}
			}
		}
		else {
			// Backpropogate the rates to create a prediction
			predData.r.backward();
			// Set the frame from this prediction
			predData.setFromRate();
		}




	}

	if (!bLinked) {




	}



	// Increment the frames we've seen
	nFramesSinceObs = data.bValid ? 1 : (nFramesSinceObs + 1);

	// Output data is valid; save the output data
	outData.bValid = true;
	data = outData;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
