#pragma once

#include "ofMain.h"
#include "ofxFilterUtils.h"
#include "ofxFilterUnits.h"

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
    class RateForwardParams {
    public:
        // How much do we ease new rates? [0, 1)
        // 0    no easing
        // 0.5  half old, half new rate
        // 0.8  4/5 old, 1/5 new rate (good all-around param)
        // 0.99 mostly old, very little new (changes very, very slowly)
        // Easing params change depending on speed. The idea behind this is
        // that higher speeds should allow rates to change faster, but
        // lower speeds should change rates slower (this helps with
        // high-frequency noise in the higher-order rates).
        float fastEaseParam = 0.1;      // should be less
        float slowEaseParam = 0.95;     // should be more
        // The default param is used when speed is not available.
        float defaultEaseParam = 0.8;
        // We calculate the appropriate param by comparing the last
        // speed to reference speeds, which are the maximum speeds for
        // each channel (translation, rotation, scale).
        glm::vec3 maxSpeed = { 1.0, 90.0, 1.0 };
        // The ratePower describes how much more easing
        // is applied to rates, the higher their order. (0, +inf)
        // 1 = no difference
        // 0.5 = acc eases more than vel
        // 2 = acc eases less than vel (not recommended)
        float easeParamRatePower = 1.0;
    };
    void forward(glm::mat4 m, RateForwardParams& p, int nFrames = 1);
    
	// Backpropogate the rate to yield a prediction
	void backward(int nFrames = 1);
	
	// Apply a frictional force.
    // Friction describes the fraction of energy that is retained
    // in higher order rates every frame.
    class RateFrictionParams {
    public:
        
        // Friction describes the amount of energy retained each
        // step (frame) in a rate.
        float friction = 0.95;
        
        // Friction gets progressively higher for higher order
        // rates by a power function.
        // range: [-1, +inf)
        // -1   none
        //          vel *=  friction^0
        //          acc *=  friction^0
        //          jerk *= friction^0
        // 0    constant
        //          vel *=  friction^1
        //          acc *=  friction^1
        //          jerk *= friction^1
        // 1    linearly increasing with order
        //          vel *=  friction^1
        //          acc *=  friction^2
        //          jerk *= friction^3
        // 2    exponentially increasing by a power of 2
        //          vel *=  friction^1
        //          acc *=  friction^3
        //          jerk *= friction^5
        float ratePower = 2.0;
    };
	void applyFriction(RateFrictionParams& p);
    
    // Reduce higher order rates (acc+) the more they oppose the next
    // lower rate. This can be useful when switching from converging
    // to predicting only, since converging can introduce rates
    // of higher-than normal magnitude, which, when stopped immediately,
    // result in strange behaviors. This function is essentially a check
    // on convergence.
    class RateReduceParams {
    public:
        float opposingDirMult = 0; // reduction for opposing directions
        float alignedDirMult = 1;  // reduction for aligned directions
        float power = 1.0;         // sensitizes the multiplier for the right orthogonal multiplier
    };
    void reduceRates(RateReduceParams& p);
    

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
	// Which of these measures is valid? (being operated upon)
	// This must be set externally.
	glm::bvec3 validMeasures = glm::bvec3(false, false, false);

	// Lerp to another data object
	void lerp(ofxFilterData& to, float amt);

	// Compare for similarity to another data object
	class SimilarityParams {
	public:
		glm::vec3 thresh = { 0.01, 3, 0.01 };	// thresholds of t[0], r[0], s[0]
		glm::vec3 mix = { 1, 0, 0 };			// mix of t, r, s
		int nRates = 1;							// how many rates to include (1 = t[0],r[0],s[0] only)
		float rateThreshMult = 10.0;			// how much are the rate thresholds multiples of the original threshold
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
		// Maximum reasonable speed we would want a point to move (for t, r, s)
		glm::vec3 maxSpeed = { 2.0, 90.0, 1.0 };
		float approachTime = 1.0;	// how many seconds until we begin slowing down for the approach?
		float approachBuffer = 0.1; // how close until the target is the target speed the observed speed
		float accStepPower = 1.25;
        float targetSpeedEaseParam = 0.5;   // range: [0, 1); 0 = no easing of the target speed
        float accMagEaseParam = 0.995; // how fast does the magnitude of the acceleration ease? range: [0, 1); 0 = no easing (sharp corners), 0.995 = rounded corners
	};
	bool converge(ofxFilterData& to, ConvergenceParams& p);


	// ----------------------------

	// Rate Parameters
	mat4rate r;
    
    // Update the rates from the current frame m
    void updateRateFromFrame(int nElapsedFrames, mat4rate::RateForwardParams& p);
    
	// Set the internal matrix from the rate
	bool setFrameFromRate();


protected:

};
