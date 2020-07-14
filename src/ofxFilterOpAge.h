#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"

class ofxFilterOpAgeSettings : public ofxFilterOpSettings {
public:

	ofxFilterOpAgeSettings() {
		type = "age";
		abbr = "AGE";
	};
	~ofxFilterOpAgeSettings() {};

	void setupParams();

	// What is the minimum age data must be in order to be exported?
	// Data less than this age will not be passed through.
	// Data larger than or equal to this age will be passed through.
	// For example, if minAge = 4, then on the 4th frame (1,2,3,>>4<<),
	// will data allowed to be validated.
	// (Note: The age counter does not look for consecutive validations.)
	// (This value is in frames)
	int minAge = 4;


protected:

	long _maxLifespan() { return max(minAge, 0) + 2; }

};

class ofxFilterOpAge : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);
	
	bool bThereWasValidData = false;

	// What is the age (in frames) of this data?
	uint64_t age = 0;

	void _clear() {
		age = 0;
	}

};
