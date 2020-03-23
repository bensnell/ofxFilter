#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"

class ofxFilterOpAddRateSettings : public ofxFilterOpSettings {
public:

	ofxFilterOpAddRateSettings() {
		type = "add-rate";
	};
	~ofxFilterOpAddRateSettings() {};

	void setupParams();

	// What is the rate order to be calculated?
	// 1 = none
	// 2 = velocity
	// 3 = velocity, acceleration
	// 4 = velocity, acceleration, jerk
	int order = 3;

	bool bResetRateAfterEmptyData = false;

};

class ofxFilterOpAddRate : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

	// Apply this operator to data and get transformed data as output
	void process(ofxFilterData& data);


protected:

	ofxFilterData lastData;

	int nFramesSinceLastValidData = -1;

};