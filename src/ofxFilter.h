#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"

enum ofxFilterMode {
	FILTER_NONE = 0,
	FILTER_KALMAN,
	FILTER_EASING,
	NUM_FILTERS
};

class ofxFilter {
public:
    
    ofxFilter();
    ~ofxFilter();

	// Set this filter's mode
	void setMode(ofxFilterMode _mode);
	// Get the names of the filter modes
	static vector<string> getModeNames() { return { "none", "kalman", "easing" }; }

	// Set params
	void setParamsKalman(float smoothness, float rapidness, bool bUseAccel = false, bool bUseJerk = false);
	void setParamsEasing(float _easingParam);
    
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

	ofxFilterMode mode = FILTER_KALMAN;

	// Last samples
	glm::mat4x4 lastFrame;

	// Frame utils 
	ofxCv::KalmanPosition kalmanPosition;
	ofxCv::KalmanEuler kalmanOrientation;

	float easingParam = 0.95;
	glm::vec3 easedPosition;
	glm::quat easedQuaternion;
	bool bFirstEase = true;
};

void decomposeMat4x4(glm::mat4x4& _mat, glm::vec3& _outPosition, glm::quat& _outQuaternion);
void composeMat4x4(glm::vec3& _position, glm::quat& _quaternion, glm::mat4x4& _outMat);
glm::quat quatConvert(ofQuaternion& _q);
glm::vec3 getTranslation(glm::mat4x4& a);
glm::vec3 getXAxis(glm::mat4x4& a);
glm::vec3 getYAxis(glm::mat4x4& a);
glm::vec3 getZAxis(glm::mat4x4& a);