#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"

class ofxFilterOpPersistSettings : public ofxFilterOpSettings {
public:

	ofxFilterOpPersistSettings() {
		type = "persist";
		abbr = "PST";
	};
	~ofxFilterOpPersistSettings() {};

	void setupParams();

	// This operator forces the data to be valid.
	// For how many invalid frames should this done?
	// Possible values:
	//		-1 for an infinite number of frames
	//		0 will disable this operatore
	//		n for n frames
	int	nFrames = 0;


protected:

	long _maxLifespan() { return max(nFrames, 0) + 2; }

};

class ofxFilterOpPersist : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);
	
	// How many consecutive invalid frames have passed?
	uint64_t nInvalidFrames = 0;

	void _clear() {
		nInvalidFrames = 0;
	}

};
