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

	// At what rate order will we begin exporting?
	// 0	at a position
	// 1	at a velocity
	// 2	at an acceleration
	int rateOrderToBeginExport = 1;

	// When the prediction is linked to observed, are the prediction's
	// rates copied from the observation or derived from the 
	// observation's frame only?
	ofxFilterData::ReconciliationMode linkReconMode = ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL;

	// After how many frames will the predictions and
	// observations unlink?
	int nFramesUnlinkThresh = 10;

	// Should similarity be a factor in unlinking?
	// TODO



	// Friction applied to all rate parameters
	// TODO (?): Applied differentially to higher-order rates?
	float friction = 0.95;

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