#include "ofxFilterOpContinuity.h"

// --------------------------------------------------
void ofxFilterOpContinuitySettings::setupParams() {

	RUI_SHARE_PARAM_WCN(getID() + "- Max Pred Frames", nMaxPredFrames, 0, 14400);
	RUI_SHARE_PARAM_WCN(getID() + "- Rate Order for Export", rateOrderToBeginExport, 0, 3);
	vector<string> reconModes = ofxFilterData::getReconciliationModes();
	RUI_SHARE_ENUM_PARAM_WCN(getID() + "- Linked Recon Mode", existingLinkReconMode, ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL, ofxFilterData::ReconciliationMode::NUM_OFXFILTERDATA_RECONCILE_MODES-1, reconModes);
	RUI_SHARE_PARAM_WCN(getID() + "- Frames to Unlink", nFramesUnlinkThresh, 0, 120);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Trans Thresh", simParams.thresh[0], 0, 10);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Trans Mix", simParams.mix[0], 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Rot Thresh", simParams.thresh[1], 0, 45);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Rot Mix", simParams.mix[1], 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Scale Thresh", simParams.thresh[2], 0, 10);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Scale Mix", simParams.mix[2], 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Num Rates", simParams.nRates, 0, 4);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Rate Thresh Mult", simParams.rateThreshMult, 0, 100);
	RUI_SHARE_PARAM_WCN(getID() + "- Sim Rate Weight", simParams.rateWeight, 0, 10);
	RUI_SHARE_PARAM_WCN(getID() + "- Friction", friction, 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Lookahead Frames", nLookaheadFrames, 0, 30);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv FPS", convParams.frameRate, 0, 500);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Max Trans Speed", convParams.maxSpeed[0], 0, 20);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Max Rot Speed", convParams.maxSpeed[1], 0, 360);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Max Scale Speed", convParams.maxSpeed[2], 0, 20);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Approach Time", convParams.approachTime, 0, 20);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Approach Buf", convParams.approachBuffer, 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Acc Step Power", convParams.accStepPower, 1, 10);


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

	// Determine whether we are currently exporting data
	if (bExporting) {
		// If this data is invalid and the max number of frames have elapsed, stop exporting data
		if (!data.bValid && nFramesSinceObs >= s->nMaxPredFrames) {
			bExporting = false;
		}
	}
	else {
		// If the observed data is valid and the required rate is present,
		// then begin exporting data
		if (data.bValid && data.r.b[min(s->rateOrderToBeginExport, data.r.size())]) {
			bExporting = true;
		}
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
				tmpData.setFrameFromRate();

				// If many frames have elapsed and the observed data differs significantly from 
				// predicted data, then this can no longer be linked. Proceed to unlinked
				// instructions
				if (nFramesSinceObs > s->nFramesUnlinkThresh && !tmpData.similar(data, s->simParams)) {
					bLinked = false;
				}
				else {
					// Reconcile new (known) data with previous (perhaps uncertain) data.
					predData.reconcile(data, s->existingLinkReconMode);
				}
			}
		}
		else {
			// Backpropogate the rates to create a prediction
			predData.r.backward();
			// Set the frame from this prediction
			predData.setFrameFromRate();
		}
	}

	if (!bLinked) {

		if (data.bValid) {
			// Valid data coming in that must be reconciled with
			
			// Look ahead a number of frames to find the likely observed point
			// that we will attempt to converge onto
			tmpData = data;
			for (int i = 0; i < s->nLookaheadFrames; i++) {
				tmpData.r.backward();
				if (i == (s->nLookaheadFrames - 1)) tmpData.setFrameFromRate();
			}

			// Converge the rates to this frame
			tmpData = predData;
			tmpData.converge(tmpData, s->convParams);

			// Generate a prediction and set this frame
			tmpData.r.backward();
			tmpData.setFrameFromRate();

			// Compare with the observed data
			if (tmpData.similar(data, s->simParams)) {
				// Our predictions are similar to reality, so link up and reconcile new data
				bLinked = true;

				// TODO: what happens when data doesn't have complete motion params but
				// the mode instructs all to be copied? should there be a backup?
				// TODO: Do we reconcile with data or with tmpData? Or is tmpData doing
				// the reconciling?
				predData.reconcile(data, s->newLinkReconMode);
			}
			else {
				// We have been operating on our predictions all along
				predData = tmpData;
			}
		}
		else {
			// No valid data, so we can only depend on predictions
			predData.r.applyFriction(s->friction);
			predData.r.backward();
			predData.setFrameFromRate();
		}
	}

	// The output data will be the predictions
	outData = predData;

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
