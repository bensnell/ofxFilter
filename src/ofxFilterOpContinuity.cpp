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
    
    linkedFrictionParams.friction = 0.95;
    linkedFrictionParams.ratePower = -1;
	RUI_SHARE_PARAM_WCN(getID() + "- Lnk Friction", linkedFrictionParams.friction, 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Lnk Friction Rate Power", linkedFrictionParams.ratePower, -1, 10);
    
    unlinkedFrictionParams.friction = 0.95;
    unlinkedFrictionParams.ratePower = 2.0;
    RUI_SHARE_PARAM_WCN(getID() + "- Ulnk Friction", unlinkedFrictionParams.friction, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- Ulnk Friction Rate Power", unlinkedFrictionParams.ratePower, -1, 10);
    
	RUI_SHARE_PARAM_WCN(getID() + "- Lookahead Frames", nLookaheadFrames, 0, 30);
    RUI_SHARE_ENUM_PARAM_WCN(getID() + "- New Link Recon Mode", newLinkReconMode, ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL, ofxFilterData::ReconciliationMode::NUM_OFXFILTERDATA_RECONCILE_MODES-1, reconModes);
    
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Epsilon Power", convParams.epsilonPower, 0, 12);
    
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Max Trans Speed", convParams.maxSpeed[0], 0, 20);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Max Rot Speed", convParams.maxSpeed[1], 0, 360);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Max Scale Speed", convParams.maxSpeed[2], 0, 20);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Approach Time", convParams.approachTime, 0, 20);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Approach Buf", convParams.approachBuffer, 0, 1);
	RUI_SHARE_PARAM_WCN(getID() + "- Conv Acc Step Power", convParams.accStepPower, 1, 10);
    RUI_SHARE_PARAM_WCN(getID() + "- Conv Target Speed Ease", convParams.targetSpeedEaseParam, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- Conv Acc Mag Ease", convParams.accMagEaseParam, 0, 1);
    
    RUI_SHARE_PARAM_WCN(getID() + "- Red Opp Dir Mult", reduceParams.opposingDirMult, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- Red Aln Dir Mult", reduceParams.alignedDirMult, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- Red Power", reduceParams.power, 0, 1);

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

    
    // =============================================
    // ============== UPDATE EXPORTING =============
    // =============================================
    
	// Determine whether we are currently exporting data
	if (bExporting) {
		// If this data is invalid and the max number of frames have elapsed, stop exporting data
		if (!data.bValid && nFramesSinceObs >= s->nMaxPredFrames) {
			bExporting = false;

			// Reset all bools
			bLinked = true;
			bSetFirstPred = false;
            nFramesSinceObs = 1;
            bLastDataValid = false;
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
    
    
    // =============================================
    // =============== UPDATE LINKING ==============
    // =============================================
    
    // Update linking
    bool bLinkedLast = bLinked;
    if (bLinked && nFramesSinceObs > s->nFramesUnlinkThresh) {  // check if we should unlink

        if (!data.bValid) {
            // Stop linking if we are over threshold
            bLinked = false;

        } else {
            // What would the current prediction be if we proceed without observations?
            tmpData = predData;
            // TODO (?) should rates be reduced?
            tmpData.r.applyFriction(s->linkedFrictionParams);
            tmpData.r.backward();
            tmpData.setFrameFromRate();

            // Do predictions differ significantly from observations?
            if (!tmpData.similar(data, s->simParams)) {
                // If so, data can no longer be linked. Proceed to unlinked instructions.
                bLinked = false;
            }
        }
    } else if (!bLinked && data.bValid) {                       // check if we should re-link

        // Look ahead a number of frames to find the likely observed point
        // that we will attempt to converge onto.
        tmpData = data;
        for (int i = 0; i < s->nLookaheadFrames; i++) {
            tmpData.r.applyFriction(s->unlinkedFrictionParams);
            tmpData.r.backward();
            if (i == (s->nLookaheadFrames - 1)) tmpData.setFrameFromRate();
        }

        // Converge the rates to this frame
        convData = predData;
        if (!convData.converge(tmpData, s->convParams)) {
            // If convergence is not possible, proceed as if it were,
            // using the tmpData to generate a prediction.
        }

        // Generate a prediction and set this frame
        convData.r.backward();
        convData.setFrameFromRate();

        // Compare with the observed data
        if (convData.similar(data, s->simParams)) {
            // Our predictions are similar to reality, so link up and reconcile new data
            bLinked = true;
        }
    }
    
    
    // =============================================
    // ================ UPDATE DATA ================
    // =============================================

    // Update the data
    if (data.bValid) {  // there is valid data available

        if (bLinked) {
            
            if (!bSetFirstPred) { // Set the very first data
                bSetFirstPred = true;
                predData = data;
                
            } else {
                // Reconcile new (known) data with previous (perhaps uncertain) data.
                // Reconcile data observations with predictions.
                predData.reconcile(data, (bLinkedLast ? s->existingLinkReconMode : s->newLinkReconMode));
                // TODO (?) Should this just be copying?
                // TODO (?) should new rates be generated? This doesn't copy the whole frame...
            }
        } else {
            // We have been operating on our predictions all along.
             // (convData already calculated above)
            predData = convData;
        }
        
    } else {            // there is not valid data available
        
        // Reduce rates if applicable
        // TODO (?) Should these params be different than the unlinked reduction params?
        if (bLastDataValid) predData.r.reduceRates(s->reduceParams);

        // Apply friction
        predData.r.applyFriction(bLinked ? s->linkedFrictionParams : s->unlinkedFrictionParams);
        // Backpropogate the rates to create a prediction
        predData.r.backward();
        // Set the frame from this prediction
        predData.setFrameFromRate();
    }

	// The output data will be the predictions
	outData = predData;

	// Increment the frames we've seen
	nFramesSinceObs = data.bValid ? 1 : (nFramesSinceObs + 1);

    // Save whether this data was valid
    bLastDataValid = data.bValid;
    
	// Output data is valid; save the output data
	outData.bValid = true;
	data = outData;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
