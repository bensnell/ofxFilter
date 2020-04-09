#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilterUtils.h"
#include "ofxFilterOp.h"
#include "ofxFilterOpEasing.h"
#include "ofxFilterOpKalman.h"
#include "ofxFilterOpAddRate.h"
#include "ofxFilterOpContinuity.h"

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
	

private:

	// All operators (layers)
	vector<ofxFilterOp*> ops;

	// Last processed frame
	ofxFilterData frame;

	// What measures (translation, rotation, scale) are valid?
	// And by consequence, what measures need to be calculated?
	glm::bvec3 validMeasures = glm::bvec3(false, false, false);
	glm::mat4 _process(glm::mat4 in);

};
