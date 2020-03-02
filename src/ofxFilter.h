#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"

enum ofxFilterMode {
	FILTER_NONE = 0,
	FILTER_KALMAN,
	//FILTER_EASING,
	NUM_FILTER_MODES
};

enum ofxFilterPredMode {
	FILTER_PRED_NONE = 0,
	FILTER_PRED_KALMAN,
	FILTER_PRED_ACC,
	NUM_FILTER_PRED_MODES
};

enum ofxFilterPostMode {
	FILTER_POST_NONE = 0,
	FILTER_POST_EASING,
	NUM_FILTER_POST_MODES
};

class ofxFilterSettings {
public:

	// --------- NORMAL FILTER ---------
	ofxFilterMode mode = FILTER_KALMAN;

	float kalmanSmoothness = 3;
	float kalmanRapidness = 1;
	bool bKalmanUseAccel = true;
	bool bKalmanUseJerk = false;

	float easingParam = 0.96;

	// Converted/calculated params
	float getCalcKalmanSmoothness() { return 1.0 / pow(10.0, kalmanSmoothness); }
	float getCalcKalmanRapidness() { return 1.0 / pow(10.0, kalmanRapidness); }


	// -------- PREDICTION ----------

	ofxFilterPredMode predMode = FILTER_PRED_ACC;

	// After how many empty frames does prediction begin?
	//int predDelay = 0;



	// -------- POST PROCESSING --------
	ofxFilterPostMode postMode = FILTER_POST_EASING;



	float postEasingParam = 0.96;


	

};

class ofxFilter {
public:
    
    ofxFilter();
    ~ofxFilter();

	// Get the names of the filter modes
	static vector<string> getModeNames() { return { "none", "kalman", "easing" }; }

	// Set params
	void setParams(ofxFilterSettings _settings);
	void setup();

	// Add a measurement
	void add(float scalar);
	void add(glm::vec2 position);
	void add(glm::vec3 position);
	void add(glm::quat quaternion);
	void add(glm::mat4x4& frame);

	// Add a null measurement (make another prediction; move the kalman filter
	// forward one time step without a measurement)
	void add();

	// Get a prediction
	float getScalar();				// Scalar filtering
	glm::vec2 getPosition2D();		// 2D filtering
	glm::vec3 getPosition();		// 3D filtering
	glm::quat getOrientation();		// Quaternion filtering
	glm::mat4x4 getFrame();			// Frame filtering (position, orientation, no scaling)


private:

	// These are all of the filter's parameters
	ofxFilterSettings settings;

	// Initialize
	void initialize();

	// Last samples
	glm::mat4x4 lastMeasurement;
	glm::mat4x4 thisFrame;
	glm::mat3x3 linearMotionParams; // position, velocity, acceleration

	// Frame utils 
	ofxCv::KalmanPosition kalmanPosition;
	ofxCv::KalmanEuler kalmanOrientation;

	glm::vec3 easedPosition;
	glm::quat easedQuaternion;
	bool bFirstEase = true;

	int nEmptyMeasurements = 0;

	glm::mat4x4 getFilteredFrame();
};

void decomposeMat4x4(glm::mat4x4& _mat, glm::vec3& _outPosition, glm::quat& _outQuaternion);
void composeMat4x4(glm::vec3& _position, glm::quat& _quaternion, glm::mat4x4& _outMat);
glm::quat quatConvert(ofQuaternion& _q);
glm::vec3 getTranslation(glm::mat4x4& a);
glm::vec3 getXAxis(glm::mat4x4& a);
glm::vec3 getYAxis(glm::mat4x4& a);
glm::vec3 getZAxis(glm::mat4x4& a);