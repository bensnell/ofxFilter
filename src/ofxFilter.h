#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxCv.h"

enum ofxFilterMode {
	FRAME_FILTER_NONE = 0,
	FRAME_FILTER_KALMAN,
	FRAME_FILTER_EASING,
	NUM_FRAME_FILTERS
};

class ofxFilter {
public:
    
    ofxFilter();
    ~ofxFilter();

	// Set this filter's mode
	void setMode(ofxFilterMode _mode);
	// Get the names of the filter modes
	static vector<string> getModeNames() { return { "none", "kalman", "easing" }; }
    
	// Frame filtering (position, orientation, no scaling)
	void addFrame(glm::mat4x4& _m);
	glm::mat4x4 getFrame();

	// Scalar filtering
	void addScalar(float _s);
	float getScalar();

	// Set params
	void setParamsKalman(float smoothness, float rapidness, bool bUseAccel = false);
	void setParamsEasing(float _easingParam);


private:

	ofxFilterMode mode = FRAME_FILTER_KALMAN;

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

#endif /* ofxFilter_hpp */