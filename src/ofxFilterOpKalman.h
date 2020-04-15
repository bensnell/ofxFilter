#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"
#include "ofxCv.h"

class ofxFilterOpKalmanSettings : public ofxFilterOpSettings {
public:

	ofxFilterOpKalmanSettings() {
		type = "kalman";
	};
	~ofxFilterOpKalmanSettings() {};

	void setupParams();

	float smoothness = 3;
	float rapidness = 1;
	bool bUseAccel = true;
	bool bUseJerk = false;

	// Converted/calculated params
	float getCalcSmoothness() { return 1.0 / pow(10.0, smoothness); }
	float getCalcRapidness() { return 1.0 / pow(10.0, rapidness); }

	// Make predictions for empty measurements using the Kalman filter
	bool bPredictEmpty = false;
	// How many empty predictions should be made?
	// After this many predictions, the algorithm stops predicting empty data.
	// -1 indicates that prections happen indefinitely
	int nMaxEmptyPredictions = 1;
	// Reset the filters after a stretch of absent readings
	bool bResetAfterEmpty = true;


};

class ofxFilterOpKalman : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);
 
	ofxCv::KalmanPosition kt;	// kalman translation
	ofxCv::KalmanEuler kr;		// kalman rotation
	ofxCv::KalmanPosition ks;	// kalman scale

	// Initialize the filters (resets their states)
	void initFilters();
	bool bInitFilters = false; // Have filters been initialized?

	int nEmptyPred = 0;

};