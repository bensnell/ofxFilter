#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterUnits.h"

// This class holds the settings for a filter operator
class ofxFilterOpSettings {
public:

	ofxFilterOpSettings() {};
	~ofxFilterOpSettings() {};

	// Setup the name of these settings and the depth of the operator
	void setup(string _name, int _depth) {
		name = _name;
		depth = _depth;
		ID = name + "_" + type + "_" + ofToString(depth);

		setupParams();
	};
	string getID() { return ID; };
	string getType() { return type; };
	int getDepth() { return depth; }

	virtual void setupParams();

	// What is the maximum lifespan of this operator (in # frames)?
	// By default, it is 1 frame without valid data.
	// -1 will achieve an "infinite" lifespan effect.
	virtual long maxLifespan() { return 1; }

protected:

	// Operator Type
	string type = "none";
	// Layer Name
	string name = "";
	// Depth of layer (first is 0, next is 1, ...)
	int depth = 0;
	// ID of this layer
	string ID = "";

};

// This class represents a filter operator.
// This operator can be stacked, mixed, interchanged with other operators
// to manupulate realtime streams of positional data.
class ofxFilterOp {
public:

	ofxFilterOp() {};
	~ofxFilterOp() {};

	// Setup this operator
	virtual void setup(ofxFilterOpSettings* _settings);

	// Apply this operator to data and get transformed data as output
	virtual void process(ofxFilterData& data);

	virtual void clear() {};
	
//protected:

	// Parameters
	ofxFilterOpSettings* settings;


};
