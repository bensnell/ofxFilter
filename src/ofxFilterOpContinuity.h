#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"

class ofxFilterOpContinuitySettings : public ofxFilterOpSettings {
public:

	ofxFilterOpContinuitySettings() {
		type = "continuity";
	};
	~ofxFilterOpContinuitySettings() {};

	void setupParams();

	// After how many frames without data will this stop exporting?
	int nMaxPredFrames = 240;

	// At what rate order will we begin exporting?
	// 0	at a position
	// 1	at a velocity
	// 2	at an acceleration
	int rateOrderToBeginExport = 1;

	// When the prediction is linked to observed, are the prediction's
	// rates copied from the observation or derived from the 
	// observation's frame only?
	ofxFilterData::ReconciliationMode existingLinkReconMode = ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL;

	// After how many frames will the predictions and
	// observations unlink?
	int nFramesUnlinkThresh = 10;

	// These parameters determine how similarity is calculated
	// between two different data frames.
	ofxFilterData::SimilarityParams simParams;
    
    // Friction params
    mat4rate::RateFrictionParams linkedFrictionParams;
    mat4rate::RateFrictionParams unlinkedFrictionParams;

	// In an unlinked state, how many frames do we look ahead to 
	// reconcile our current heading with?
	// 0	don't look ahead (safest)
	// n	look ahead n frames (too high and there may be artifacts)
    // TODO: lookahead changes (high when far away; low when close) (?)
	int nLookaheadFrames = 0;

	// How do we reconcile a new link?
	// TODO: Do we carryover convergence rates for a new link? Or use the
	// last predicted rates?
	// How are new links' predicted data reconciled with existing
	// data?
	ofxFilterData::ReconciliationMode newLinkReconMode = ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL;

	// Convergence params
	ofxFilterData::ConvergenceParams convParams;
    
    // Rate Reduction Params
    mat4rate::RateReduceParams reduceParams;

protected:

	long _maxLifespan() { return nMaxPredFrames + 1; }

};

class ofxFilterOpContinuity : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);

	ofxFilterData predData;	// predicted data
	ofxFilterData tmpData;	// temporary data
	ofxFilterData convData;	// converged data (if available)
	ofxFilterData outData;	// output data (final)


	// Is this operator currently exporting data (valid data)?
	bool bExporting = false;

	// Is the prediction currently linked to output?
	// If true, then there is no prediction underway. The output is the 
	// observation.
	bool bLinked = true;

	// Have we set the very first prediction?
	bool bSetFirstPred = false;

	// Number of frames since the last observation
	int nFramesSinceObs = 1;

    // Should we adjust acceleration? This is the same as whether
    // the last data was valid
    bool bLastDataValid = false;

	void _clear() {
		bExporting = false;
		bLinked = false;
		bSetFirstPred = false;
		nFramesSinceObs = 1;
		bLastDataValid = false;
	}
};
