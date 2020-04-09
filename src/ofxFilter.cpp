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
		}
		else if (type == "easing") {
			op = new ofxFilterOpEasing();
		}
		else if (type == "kalman") {
			op = new ofxFilterOpKalman();
		}
		else if (type == "add-rate") {
			op = new ofxFilterOpAddRate();
		}
		else if (type == "continuity") {
			op = new ofxFilterOpContinuity();
		}
		else {
			ofLogError("ofxFilter") << "Operator type \"" << type << "\" is not valid.";
			continue;
		}
		op->setup(_settings[i]);
		ops.push_back(op);
	}
}

// --------------------------------------------------
float ofxFilter::process(float in) {
	validMeasures[0] |= true;
	glm::mat4 m = glm::translate(glm::vec3(in, 0, 0));
	_process(m);
	return getScalar();
}

// --------------------------------------------------
glm::vec2 ofxFilter::process(glm::vec2 in) {
	validMeasures[0] |= true;
	glm::mat4 m = glm::translate(glm::vec3(in.x, in.y, 0));
	_process(m);
	return getPosition2D();
}

// --------------------------------------------------
glm::vec3 ofxFilter::process(glm::vec3 in) {
	validMeasures[0] |= true;
	glm::mat4 m = glm::translate(in);
	_process(m);
	return getPosition();
}

// --------------------------------------------------
glm::quat ofxFilter::process(glm::quat in) {
	validMeasures[1] |= true;
	glm::mat4x4 rotMat = glm::toMat4(in);
	_process(rotMat);
	return getOrientation();
}

// --------------------------------------------------
glm::mat4 ofxFilter::process(glm::mat4 in) {
	validMeasures = glm::vec3(true, true, true);
	_process(in);
	return getFrame();
}

// --------------------------------------------------
glm::mat4 ofxFilter::_process(glm::mat4 in) {
	frame.m = in;
	frame.bValid = true;
	frame.validMeasures = validMeasures;
	for (int i = 0; i < ops.size(); i++) {
		ops[i]->process(frame); // TODO: lock so we aren't in the middle of changing data
	}
	return frame.m;
}

// --------------------------------------------------
glm::mat4 ofxFilter::process() {
	// Use the last data (frame)
	frame.bValid = false;
	frame.validMeasures = validMeasures;
	for (int i = 0; i < ops.size(); i++) {
		ops[i]->process(frame);
	}
	return frame.m; // should this return null if it's invalid?
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
