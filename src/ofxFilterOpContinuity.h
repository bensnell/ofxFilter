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

	// Similarity threshold for unlinking. (0, 1]
	// Any similarity below this level must be present for unlinking.
	// 1	similarity does not factor into unlinking
	// 0.1	there must be a large amount of dissimilarity present 
	//			in order to unlink
	// 0	cannot unlink (do not set to zero)
	float dissimThreshToUnlink = 1.0;

	// These parameters determine how similarity is calculated
	// between two different data frames.
	ofxFilterData::SimilarityParams simParams;

	// In an unlinked state, how much friction will be applied?
	// Friction describes the fraction of energy that is retained
	// in higher order rates every frame.
	// TODO: Should this be applied differentially to higher order rates?
	float friction = 0.99;

	// In an unlinked state, how many frames do we look ahead to 
	// reconcile our current heading with?
	// 0	don't look ahead
	// n	look ahead n frames
	int nLookaheadFrames = 1;

	// How do we reconcile a new link?
	// TODO: Do we carryover convergence rates for a new link? Or use the
	// last predicted rates?
	// How are new links' predicted data reconciled with existing
	// data?
	ofxFilterData::ReconciliationMode newLinkReconMode = ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL;

	// Convergence params
	ofxFilterData::ConvergenceParams convParams;

};

class ofxFilterOpContinuity : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

	// Apply this operator to data and get transformed data as output
	void process(ofxFilterData& data);




protected:

	ofxFilterData predData;	// predicted data
	ofxFilterData tmpData;	// temporary predicted data
	ofxFilterData outData;	// output data (final)


	// Is this operator currently exporting data (valid data)?
	bool bExporting = false;

	// Is the prediction currently linked to output?
	// If true, then there is no prediction underway. The output is the 
	// observation.
	bool bLinked = true;

	// Number of frames since the last observation
	int nFramesSinceObs = 1;




};