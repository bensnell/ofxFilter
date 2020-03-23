#pragma once

#include "ofMain.h"
#include "ofxFilterUtils.h"

// transformation matrix rate
class mat4rate {
public:

	vector<bool> b;			// valid rate?
	vector<glm::vec3> t;	// translation rate
	vector<glm::vec3> r;	// rotation rate (euler angles)
	vector<glm::vec3> s;	// scale rate
	int size() { return b.size(); }

	void init(int size) {
		b.clear();
		b.resize(size, false);
		t.clear();
		t.resize(size, { 0,0,0 });
		r.clear();
		r.resize(size, { 0,0,0 });
		s.clear();
		s.resize(size, { 0,0,0 });
	}

	
	// Apply nFrames forward passes to update the rate
	void forward(glm::mat4 m, int nFrames = 1);
	// Backpropogate the rate to yield a prediction
	void backward(int nFrames = 1);
	
	// Apply a frictional force
	void applyFriction(float friction);

};

class ofxFilterData {
public:

	// Is this data valid (or is it empty?)
	bool bValid = true;

	// Frame data
	glm::mat4 m;

	// Set the transformation matrix
	void set(glm::mat4 _m) { m = _m; }
	void set(glm::vec3& translation, glm::quat& rotation, glm::vec3 scale) {
		m = glm::translate(translation)* glm::toMat4(rotation)* glm::scale(scale);
	}

	// Get values from the transformation matrix
	glm::vec3 translation();
	glm::quat rotation();
	glm::vec3 scale();

	// Lerp to another data object
	void lerp(ofxFilterData& to, float amt);

	// Compare for similarity to another data object
	bool similar(ofxFilterData& a, float et, float er, float es);

	// Reconcile one piece of data with another
	enum ReconciliationMode {
		OFXFILTERDATA_RECONCILE_COPY_ALL = 0,
		OFXFILTERDATA_RECONCILE_COPY_FRAME,
		OFXFILTERDATA_RECONCILE_COPY_FRAME_AND_UPDATE_RATE,
		NUM_OFXFILTERDATA_RECONCILE_MODES
	};
	vector<string> getReconciliationModes();
	void reconcile(ofxFilterData& a, ReconciliationMode mode);

	// ----------------------------

	// Rate Parameters
	mat4rate r;

	// Update the rate (motion params), using internal frame data.
	void updateRate(int nElapsedFrames = 1);
	// Set the internal matrix from the rate
	bool setFromRate();


protected:

};
