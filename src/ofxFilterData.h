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
	vector<glm::vec3>& operator[](int i) {
		switch(i) {
		case 0: return t;
		case 1: return r;
		case 2: return s;
		default:
			ofLogError("mat4rate") << "Cannot access with index " << i;
			return t; // NOT IDEAL -- what to do?
		}	
	}
	bool isOrderValid(int i) {
		if (i < 0 || i >= size()) return false;
		return b[i];
	}

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

	
	// Apply nFrames forward passes to update the rate.
	// By default, there is no easing.
	void forward(glm::mat4 m, int nFrames = 1, float easeParam = 0.0);
	// Backpropogate the rate to yield a prediction
	void backward(int nFrames = 1);
	
	// Apply a frictional force
	// Rate power describes how much friction increases for higher order rates.
	// If rate power = 0, then no friction is applied
	// velocity *= friction^(ratePower^0)
	// acceleration *= friction^(ratePower^1)
	// jerk *= friction^(ratePower^2)
	void applyFriction(float friction, float ratePower);

};

class ofxFilterData {
public:

	// Is this data valid (or is it empty?)
	bool bValid = true;

	// Frame data
	glm::mat4 m;

	// Set the transformation matrix
	void set(glm::mat4 _m) { m = _m; }
	void set(glm::vec3 translation, glm::quat rotation, glm::vec3 scale) {
		m = glm::translate(translation)* glm::toMat4(rotation)* glm::scale(scale);
	}

	// Get values from the transformation matrix
	glm::vec3 translation();
	glm::quat rotation();
	glm::vec3 scale();

	// Lerp to another data object
	void lerp(ofxFilterData& to, float amt);

	// Compare for similarity to another data object
	class SimilarityParams {
	public:
		glm::vec3 thresh = { 0.01, 3, 0.01 };	// thresholds of t[0], r[0], s[0]
		glm::vec3 mix = { 1, 0, 0 };			// mix of t, r, s
		int nRates = 1;							// how many rates to include (1 = t[0],r[0],s[0] only)
		float rateThreshMult = 10.0;			// how much are the rate thresholds muliplies of the original threshold
		float rateWeight = 0;					// 0 = no weighting; 1 = linearly decreasing, etc. (how different rates are mixed)
	};
	bool similar(ofxFilterData& d, SimilarityParams& p);

	// Reconcile one piece of data with another.
	// For example, How are linked and unlinked observations and 
	// predictions reconciled?
	// 0	predicted data = observed data (both frame and rate)
	// 1	predicted frame = observed frame; predicted r0 = observed r0
	//			(both frame and first rate)
	// 2	predicted frame = observed frame; predicted rate updated
	//			(frame used to update rate)
	enum ReconciliationMode {
		OFXFILTERDATA_RECONCILE_COPY_ALL = 0,
		OFXFILTERDATA_RECONCILE_COPY_FRAME,
		OFXFILTERDATA_RECONCILE_COPY_FRAME_AND_UPDATE_RATE,
		NUM_OFXFILTERDATA_RECONCILE_MODES
	};
	static vector<string> getReconciliationModes() {
		return { "Copy All", "Copy Frame", "Copy Frame and Update Rate" };
	}
	void reconcile(ofxFilterData& a, ReconciliationMode mode);

	// Converge rates: adjust these rates so that they attempt to 
	// approach the provided frame. Calling convergence every frame on 
	// the same frame will adjust these rates so that they "propel"
	// this closer to the provided frame.
	class ConvergenceParams {
	public:
		float epsilonPower = 5;
		float frameRate = 240.0;	// frames per second
		// Maximum reasonable speed we would want a point to move (for t, r, s)
		glm::vec3 maxSpeed = { 1.0, 90.0, 1.0 };
		float approachTime = 1.0;	// how many seconds until we begin slowing down for the approach?
		float approachBuffer = 0.05; // how close until the target is the target speed the observed speed
		//glm::vec3 accStep;			// max accelerations for t, r, s
		float accStepPower = 2.0;

	};
	bool converge(ofxFilterData& to, ConvergenceParams& p);


	// ----------------------------

	// Rate Parameters
	mat4rate r;

	// Update the rate (motion params), using internal frame data.
	void updateRateFromFrame(int nElapsedFrames = 1, float easeParam = 0);
	// Set the internal matrix from the rate
	bool setFrameFromRate();


protected:

};
