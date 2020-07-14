#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilterUtils.h"
#include "ofxFilterOp.h"
#include "ofxFilterOpEasing.h"
#include "ofxFilterOpKalman.h"
#include "ofxFilterOpAddRate.h"
#include "ofxFilterOpContinuity.h"
#include "ofxFilterOpAxes.h"
#include "ofxFilterOpAge.h"
#include "ofxFilterOpPersist.h"

// A filter manipulates realtime data using a series of ops (operations)
class ofxFilter {
public:
    
    ofxFilter();
    ~ofxFilter();

	// Setup this filter. Use the settings to determine which layers
	// and how many should be used
	void setup(vector<ofxFilterOpSettings*>& _settings);

	// Process a value and receive the processed value
	float process(float in);
	glm::vec2 process(glm::vec2 in);
	glm::vec3 process(glm::vec3 in);
	glm::quat process(glm::quat in);
	glm::mat4 process(glm::mat4 in);

	// Process an absent measurement
	glm::mat4 process();

	// Get the last processed value
	float getScalar();
	glm::vec2 getPosition2D();
	glm::vec3 getPosition();
	glm::quat getOrientation();
	glm::vec3 getFrameScale();
	glm::mat4 getFrame() { return frame.m; }

	// Is the last processed value valid?
	bool isDataValid() { return frame.bValid; }
	
	// Flags for keeping track of whether this filter has been processed.
	bool wasProcessed() { return bProcessed; }
	void resetProcessFlag() { bProcessed = false; }

	// What are the last times at which valid data was seen, either
	// as input or output? (in milliseconds)
	//uint64_t& getLastValidInputTime() { return lastValidInput; }
	//uint64_t& getLastValidOutputTime() { return lastValidOutput; }
	// How many frames of invalid input have passed?
	uint64_t& getNumInvalidOutputs() { return nInvalidOutputs; }

	// Delete all operators. Reset.
	void clear();

private:

	// All operators (layers)
	vector<ofxFilterOp*> ops;

	// Last processed frame
	ofxFilterData frame;

	// What measures (translation, rotation, scale) are valid?
	// And by consequence, what measures need to be calculated?
	glm::bvec3 validMeasures = glm::bvec3(false, false, false);

	// Process the frame, as it has been set by other process(x) calls
	void processFrame();

	// Has the last frame been processed with valid data?
	bool bProcessed = false;

	// What is the last time valid data was processed?
	//uint64_t lastValidInput = 0;
	//uint64_t lastValidOutput = 0;
	uint64_t nInvalidOutputs = 0;

};
