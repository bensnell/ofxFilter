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

	// First, add the depth of this setting
	void addDepth(int _depth) {
		depths.push_back(_depth);
	}

	// Second, these settings
	void setup(string _name) {
		if (!name.empty() && name.compare(_name) != 0) {
			ofLogWarning("ofxFilterOpSettings") << "Wrappable operators must be setup with the same name. Using the old name.";
		}
		name = _name;
		ID = name + "_" + type + "_" + getDepth();
		IDA = name + "_" + abbr + "_" + getDepth();

		setupParams();
	};
	string getID() { return ID; };
	string getIDA() { return IDA; }; // abbreviated ID
	string getType() { return type; };
	string getAbbr() { return abbr; }; // get abbreviation
	string getDepth() {
		string out = "";
		for (int i = 0; i < depths.size(); i++) {
			if (i != 0) out += "/";
			out += ofToString(depths[i]);
		}
		return out;
	}

	virtual void setupParams();

	// What is the maximum lifespan of this operator (in # frames)?
	// This should only be changed if an operator is capable of
	// making invalid data valid (e.g. "kalman", "continuity", "persist") or if an operator
	// can keep data invalid for a number of frames before becoming valid (e.g. "age").
	// By default, it is 0 frames without valid data.
	// -1 will achieve an "infinite" lifespan effect.
	long maxLifespan() { 
		return bEnabled ? _maxLifespan() : 0;
	}

	// Is this operator enabled?
	bool bEnabled = true;

protected:

	// Operator Type
	string type = "none";
	// Abbreviated operator type (3 letters)
	string abbr = "NON";
	// Layer Name
	string name = "";
	// Depths of layer (first is 0, next is 1, ...)
	// Wrappable layers (ike Age) can have multiple depths.
	vector<int> depths;
	// ID of this layer
	string ID = "";
	// Abbreviated ID
	string IDA = "";

	virtual long _maxLifespan() { return 1; }

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

	void process(ofxFilterData& data);

	void resetProcessCount() { processCount = 0; }
	int getProcessCount() { return processCount; }

	void clear();
	
protected:

	// Apply this operator to data and get transformed data as output
	virtual void _process(ofxFilterData& data);

	// Clear any other user params
	virtual void _clear() {};

	// Parameters
	ofxFilterOpSettings* settings;

	// How many times has this been processed?
	// This will be 0 the first time it is processed, 1 the second time, 
	// and so on...
	int processCount = 0;


};
