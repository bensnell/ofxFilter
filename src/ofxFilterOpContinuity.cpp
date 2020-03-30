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

//    ofLogNotice("OC") << "\n\n================================ Frame " << ofGetFrameNum();
//    if (data.bValid) {
//        ofLogNotice("OC") << "\tObserved Pos:\t" << data.translation();
//        if (data.r.isOrderValid(1)) ofLogNotice("OC") << "\tObserved Vel:\t" << data.r[0][1] << "\t\t||vel||= " << glm::l2Norm(data.r[0][1]);
//        if (data.r.isOrderValid(2)) ofLogNotice("OC") << "\tObserved Acc:\t" << data.r[0][2] << "\t\t||acc||= " << glm::l2Norm(data.r[0][2]);;
//    } else {
//        ofLogNotice("OC") << "No observed data";
//    }
//    ofLogNotice("OC") << "\tLast Pos    :\t" << predData.translation();
//    if (predData.r.isOrderValid(1)) ofLogNotice("OC") << "\tLast Vel:\t\t" << predData.r[0][1] << "\t\t||vel||= " << glm::l2Norm(predData.r[0][1]);
//    if (predData.r.isOrderValid(2)) ofLogNotice("OC") << "\tLast Acc:\t\t" << predData.r[0][2] << "\t\t||acc||= " << glm::l2Norm(predData.r[0][2]);;

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
//        ofLogNotice("OC") << "LINKED";

        if (data.bValid) {
//            ofLogNotice("OC") << "VALID DATA";

            if (!bSetFirstPred) {
                // Set the predicted data for the very first time
                bSetFirstPred = true;
                predData = data;
            }
            else {

                // If many frames have elapsed without valid data, check if the observations
                // still align with the predictions.
                if (nFramesSinceObs > s->nFramesUnlinkThresh) {

                    // What would the current prediction be if we proceed without observations?
                    tmpData = predData;
                    tmpData.r.applyFriction(s->linkedFrictionParams); // TODO: Should this be here?
                    tmpData.r.backward();
                    tmpData.setFrameFromRate();

                    // Do predictions differ significantly from observations?
                    if (!tmpData.similar(data, s->simParams)) {
                        // If so, data can no longer be linked. Proceed to unlinked instructions.
                        bLinked = false;
                        ofLogNotice("OC") << "Stop Link...";
                    }
                }

                // If we haven't just unlinked, then reconcile new (known) data with previous
                // (perhaps uncertain) data.
//                if (bLinked) predData.reconcile(data, s->existingLinkReconMode);
                if (bLinked) {
                    predData = data;
                }

                bFlagAdjustAcc = true;
            }
        }
        else {
//            ofLogNotice("OC") << "INVALID DATA";

            // Stop linking if we are over threshold
            if (nFramesSinceObs > s->nFramesUnlinkThresh) {
                bLinked = false;
//                ofLogNotice("OC") << "Stop Link...";
            }
            else {

                // Reduce rates if applicable
                if (bFlagAdjustAcc) {
                    bFlagAdjustAcc = false;
                    predData.r.reduceRates(s->reduceParams);
                    // should these params be different than the unlinked reduction params?
                }

                // Apply friction
                predData.r.applyFriction(s->linkedFrictionParams);
                // Backpropogate the rates to create a prediction
                predData.r.backward();
                // Set the frame from this prediction
                predData.setFrameFromRate();
            }
        }
    }
    
	if (!bLinked) {
//        ofLogNotice("OC") << "NOT LINKED";
        
		if (data.bValid) { // Valid data coming in that must be reconciled with.
//            ofLogNotice("OC") << "VALID DATA";
			
			// Look ahead a number of frames to find the likely observed point
			// that we will attempt to converge onto.
			tmpData = data;
			for (int i = 0; i < s->nLookaheadFrames; i++) {
                tmpData.r.applyFriction(s->unlinkedFrictionParams);
				tmpData.r.backward();
				if (i == (s->nLookaheadFrames - 1)) tmpData.setFrameFromRate();
			}
//            ofLogNotice("OC") << "\tConverge to :\t" << tmpData.translation().x;

			// Converge the rates to this frame
			tmpData2 = predData;
            if (!tmpData2.converge(tmpData, s->convParams)) {
                // If convergence is not possible, proceed as if it were,
                // using the tmpData to generate a prediction.
            }

			// Generate a prediction and set this frame
			tmpData2.r.backward();
			tmpData2.setFrameFromRate();
            
//            ofLogNotice("OC") << "\tAcc was " << predData.r[0][2] << " and is now " << tmpData2.r[0][2];
//            ofLogNotice("OC") << "\tVel was " << predData.r[0][1] << " and is now " << tmpData2.r[0][1];
//            ofLogNotice("OC") << "\tPos was " << predData.r[0][0] << " and is now " << tmpData2.r[0][0];

			// Compare with the observed data
			if (tmpData2.similar(data, s->simParams)) {
				// Our predictions are similar to reality, so link up and reconcile new data
				bLinked = true;

				// Reconcile data observations with predictions.
				predData.reconcile(data, s->newLinkReconMode);

//                ofLogNotice("OC") << "ReLink...";
			}
			else {
				// We have been operating on our predictions all along
				predData = tmpData2;
                
                bFlagAdjustAcc = true;
			}
		}
		else {
//            ofLogNotice("OC") << "INVALID DATA";
            
            // Reduce rates if applicable
            if (bFlagAdjustAcc) {
                bFlagAdjustAcc = false;
                predData.r.reduceRates(s->reduceParams);
            }
            
			// No valid data, so we can only depend on predictions
			predData.r.applyFriction(s->unlinkedFrictionParams);
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
