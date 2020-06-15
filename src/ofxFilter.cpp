#include "ofxFilter.h"

// --------------------------------------------------
ofxFilter::ofxFilter() {

}

// --------------------------------------------------
ofxFilter::~ofxFilter() {
}

// --------------------------------------------------
void ofxFilter::setup(vector<ofxFilterOpSettings*>& _settings) {

	// Setup all layers using these settings
	for (int i = 0; i < _settings.size(); i++) {

		string type = ofToLower(_settings[i]->getType());
		ofxFilterOp* op;
		if (type == "none") {
			op = new ofxFilterOp();
			ops.insert(ops.begin() + i, op);
		}
		else if (type == "easing") {
			op = new ofxFilterOpEasing();
			ops.insert(ops.begin() + i, op);
		}
		else if (type == "kalman") {
			op = new ofxFilterOpKalman();
			ops.insert(ops.begin() + i, op);
		}
		else if (type == "add-rate") {
			op = new ofxFilterOpAddRate();
			ops.insert(ops.begin() + i, op);
		}
		else if (type == "continuity") {
			op = new ofxFilterOpContinuity();
			ops.insert(ops.begin() + i, op);
		}
		else if (type == "axes") {
			op = new ofxFilterOpAxes();
			ops.insert(ops.begin() + i, op);
		}
		else if (type == "age") {
			op = new ofxFilterOpAge();
			ops.insert(ops.begin(), op);
			ops.push_back(op);
		}
		else if (type == "persist") {
			op = new ofxFilterOpPersist();
			ops.insert(ops.begin() + i, op);
		}
		else {
			ofLogError("ofxFilter") << "Operator type \"" << type << "\" is not valid.";
			continue;
		}
		op->setup(_settings[i]);
	}
}

// --------------------------------------------------
float ofxFilter::process(float in) {

	frame.bValid = true;

	validMeasures[0] |= true;
	frame.validMeasures = validMeasures;

	frame.m = glm::translate(glm::vec3(in, 0, 0));

	processFrame();

	return getScalar();
}

// --------------------------------------------------
glm::vec2 ofxFilter::process(glm::vec2 in) {

	frame.bValid = true;

	validMeasures[0] |= true;
	frame.validMeasures = validMeasures;

	frame.m = glm::translate(glm::vec3(in.x, in.y, 0));

	processFrame();

	return getPosition2D();
}

// --------------------------------------------------
glm::vec3 ofxFilter::process(glm::vec3 in) {

	frame.bValid = true;

	validMeasures[0] |= true;
	frame.validMeasures = validMeasures;

	frame.m = glm::translate(in);

	processFrame();

	return getPosition();
}

// --------------------------------------------------
glm::quat ofxFilter::process(glm::quat in) {

	frame.bValid = true;

	validMeasures[1] |= true;
	frame.validMeasures = validMeasures;

	frame.m = glm::toMat4(in);

	processFrame();

	return getOrientation();
}

// --------------------------------------------------
glm::mat4 ofxFilter::process(glm::mat4 in) {

	frame.bValid = true;

	validMeasures = glm::vec3(true, true, true);
	frame.validMeasures = validMeasures;

	frame.m = in;

	processFrame();
	
	return getFrame();
}

// --------------------------------------------------
glm::mat4 ofxFilter::process() {
	// This processes invalid data. 
	
	frame.bValid = false;
	
	frame.validMeasures = validMeasures;

	// By default, use the last data (it doesn't matter what data is provided)

	processFrame();

	return getFrame();
}

// --------------------------------------------------
void ofxFilter::processFrame() {

	// Mark that this has been processed
	bProcessed = true;

	// If data is valid, mark this time
	//if (frame.bValid) lastValidInput = ofGetElapsedTimeMillis();

	// Reset the process counts of all operators
	for (int i = 0; i < ops.size(); i++) {
		ops[i]->resetProcessCount();
	}

	// Process the data through all operators
	for (int i = 0; i < ops.size(); i++) {
		// TODO: lock so we aren't in the middle of changing data?
		ops[i]->process(frame); 
	}

	// If output data is valid, mark this time
	//if (frame.bValid) lastValidOutput = ofGetElapsedTimeMillis();
	
	// Increment the count of the number of invalid outputs
	nInvalidOutputs = frame.bValid ? 0 : (nInvalidOutputs + 1);
}

// --------------------------------------------------
float ofxFilter::getScalar() {
	return getTranslation(frame.m).x;
}

// --------------------------------------------------
glm::vec2 ofxFilter::getPosition2D() {
	auto p = getTranslation(frame.m);
	return glm::vec2(p.x, p.y);
}

// --------------------------------------------------
glm::vec3 ofxFilter::getPosition() {
	return getTranslation(frame.m);
}

// --------------------------------------------------
glm::quat ofxFilter::getOrientation() {
	glm::mat4 m = getFrame();
	glm::vec3 p;
	glm::quat q;
	decomposeMat4x4(m, p, q);
	return q;
}

// --------------------------------------------------
glm::vec3 ofxFilter::getFrameScale() {
	return getScale(frame.m);
}

// --------------------------------------------------
void ofxFilter::clear() {

	for (int i = 0; i < ops.size(); i++) {
		if (ops[i] != NULL) {
			ops[i]->clear();
			delete ops[i];
		}
	}
	ops.clear();
	frame.clear();
	validMeasures = glm::bvec3(false, false, false);
	bProcessed = false;
	nInvalidOutputs = 0;
}

// --------------------------------------------------


// --------------------------------------------------
