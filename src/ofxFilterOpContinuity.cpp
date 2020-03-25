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
	RUI_SHARE_PARAM_WCN(getID() + "- Friction Rate Power", frictionPower, 0, 10);
	RUI_SHARE_PARAM_WCN(getID() + "- Lookahead Frames", nLookaheadFrames, 0, 30);
	RUI_SHARE_PARAM_WCN(getID() + "- Epsilon Power", convParams.epsilonPower, 0, 12);
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

	ofLogNotice("OC") << "\n\n================================ Frame " << ofGetFrameNum();
    if (data.bValid) {
        string speed = data.r.isOrderValid(1) ? ofToString(glm::l2Norm(data.r[0][1])) : "--";
        ofLogNotice("OC") << "\tObserved Pos:\t" << data.translation() << "\tspeed: " << speed; // << "\tv: " << data.r[0][1] << "\ta: " << data.r[0][2];
        speed = predData.r.isOrderValid(1) ? ofToString(glm::l2Norm(predData.r[0][1])) : "--";
        ofLogNotice("OC") << "\tLast Pos    :\t" << predData.translation() << "\tspeed: " << speed; // << "\tv: " << predData.r[0][1] << "\ta: " << predData.r[0][2];
    } else {
        ofLogNotice("OC") << "No observed data";
    }

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

			// Reset all bools
			bLinked = true;
			bSetFirstPred = false;
			ofLogNotice("OC") << "Stopped Exporting";
		}
	}
	else {
		// If the observed data is valid and the required rate is present,
		// then begin exporting data
		if (data.bValid && data.r.b[min(s->rateOrderToBeginExport, data.r.size())]) {
			bExporting = true;
			ofLogNotice("OC") << "Started Exporting";
		}
	}

    if (bExporting) ofLogNotice("OC") << "We are currently exporting";
    else ofLogNotice("OC") << "We are NOT exporrting.";

	// If we're not exporting, mark output data as invalid
	if (!bExporting) {
		data.bValid = false;
		return;
	}

	// We are currently exporting data

	if (bLinked) {

		if (data.bValid) {
			if (!bSetFirstPred) {
				// Set the predicted data for the very first time
				bSetFirstPred = true;
				predData = data;
                ofLogNotice("OC") << "Set first prediction";
			}
			else {

				// What would the current prediction be if we proceed without observations?
				tmpData = predData;
				tmpData.r.applyFriction(s->friction, s->frictionPower); // TODO: Should this be here?
				tmpData.r.backward();
				tmpData.setFrameFromRate();

                ofLogNotice("OC") << "Calc linked prediction " << tmpData.translation();

				// If many frames have elapsed and the observed data differs significantly from 
				// predicted data, then this can no longer be linked. Proceed to unlinked
				// instructions
				if (nFramesSinceObs > s->nFramesUnlinkThresh && !tmpData.similar(data, s->simParams)) {
					bLinked = false;
                    ofLogNotice("OC") << "Too dissimilar and over num frames for linkage. Stopping linkage";
				}
				else {
					// Reconcile new (known) data with previous (perhaps uncertain) data.
					predData.reconcile(data, s->existingLinkReconMode);

                    ofLogNotice("OC") << "Within linking window. Reconciling: " << predData.translation();
				}
			}
		}
		else {

			// Stop linking if we are over threshold
			if (nFramesSinceObs > s->nFramesUnlinkThresh) {
				bLinked = false;
                ofLogNotice("OC") << "Stopping linkage. Over frame threshold.";
			}
			else {
				// Apply friction
				predData.r.applyFriction(s->friction, s->frictionPower);
				// Backpropogate the rates to create a prediction
				predData.r.backward();
				// Set the frame from this prediction
				predData.setFrameFromRate();

                ofLogNotice("OC") << "Invalid data during linkage. Making prediction... " << predData.translation() << "\t" << predData.r[0][1] << "\t" << predData.r[0][2];
			}
		}
	}

	if (!bLinked) {

        ofLogNotice("OC") << "We are not linked.";
        
		if (data.bValid) {
			// Valid data coming in that must be reconciled with
            
            ofLogNotice("OC") << "There is valid data.";
			
			// Look ahead a number of frames to find the likely observed point
			// that we will attempt to converge onto
			tmpData = data;
			for (int i = 0; i < s->nLookaheadFrames; i++) {
                tmpData.r.applyFriction(s->friction, s->frictionPower);
				tmpData.r.backward();
				if (i == (s->nLookaheadFrames - 1)) tmpData.setFrameFromRate();
			}
            ofLogNotice("OC") << "\tConverge to :\t" << tmpData.translation() << " with speed " << glm::l2Norm(tmpData.r[0][1]);

			// Converge the rates to this frame
			tmpData2 = predData;
            if (!tmpData2.converge(tmpData, s->convParams)) {
                // If convergence is not possible, proceed as if it were,
                // using the tmpData to generate a prediction.
            }

			// Generate a prediction and set this frame
			tmpData2.r.backward();
			tmpData2.setFrameFromRate();
            
            ofLogNotice("OC") << "\tAcc was " << predData.r[0][2] << " and is now " << tmpData2.r[0][2];
            ofLogNotice("OC") << "\tVel was " << predData.r[0][1] << " and is now " << tmpData2.r[0][1];
            ofLogNotice("OC") << "\tPos was " << predData.r[0][0] << " and is now " << tmpData2.r[0][0];

			// Compare with the observed data
			if (tmpData2.similar(data, s->simParams)) {
				// Our predictions are similar to reality, so link up and reconcile new data
				bLinked = true;

				// TODO: what happens when data doesn't have complete motion params but
				// the mode instructs all to be copied? should there be a backup?
				// TODO: Do we reconcile with data or with tmpData? Or is tmpData doing
				// the reconciling?
				predData.reconcile(data, s->newLinkReconMode);

				ofLogNotice("OC") << "Converged data is similar, so reconcile and relink: " << predData.translation();
			}
			else {
				// We have been operating on our predictions all along
				predData = tmpData2;

				ofLogNotice("OC") << "Converged data is not similar enough... continue predicting";
			}
		}
		else {
			// No valid data, so we can only depend on predictions
			predData.r.applyFriction(s->friction, s->frictionPower);
			predData.r.backward();
			predData.setFrameFromRate();

            ofLogNotice("OC") << "Prediction: " << predData.translation() << "\t" << predData.r[0][1] << "\t" << predData.r[0][2];
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
