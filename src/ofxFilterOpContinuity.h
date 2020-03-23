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


	ofxFilterData outData;
	ofxFilterData tmpData;


	// Mixture of predicted and observed data.
	// 0.0 == 100% predicted data
	// 1.0 == 100% observed data
	float obsAmt = 1.0;



	bool bLastDataValid = false;
};