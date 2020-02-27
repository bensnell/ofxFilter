#include "ofxFDeep.h"

void decomposeMat4x4(glm::mat4x4& _mat, glm::vec3& _outPosition, glm::quat& _outQuaternion) {
	// represents translation, then rotation
	_outPosition = glm::vec3(_mat[3][0], _mat[3][1], _mat[3][2]);
	_outQuaternion = glm::quat_cast(_mat); // toQuat?
}

glm::quat quatConvert(ofQuaternion& _q) {
	return glm::quat(_q.w(), _q.x(), _q.y(), _q.z()); // w,x,y,z
}

void composeMat4x4(glm::vec3& _position, glm::quat& _quaternion, glm::mat4x4& _outMat) {
	// translation, then rotation
	glm::mat4x4 posMat = glm::translate(_position);
	glm::mat4x4 rotMat = glm::toMat4(_quaternion);
	_outMat = posMat * rotMat;
}

// --------------------------------------------------
ofxFilter::ofxFilter() {

	lastFrame = glm::translate(glm::vec3(0, 0, 0));

	kalmanPosition.init(0.001, 0.01);
	kalmanOrientation.init(0.001, 0.01);
    
}

// --------------------------------------------------
ofxFilter::~ofxFilter() {
    
}

// --------------------------------------------------
void ofxFilter::addFrame(glm::mat4x4& _m) {
	lastFrame = _m;
	switch (mode) {
	case FRAME_FILTER_NONE: {

		// nothing to do

	}; break;
	case FRAME_FILTER_EASING: {

		glm::vec3 p;
		glm::quat q;
		decomposeMat4x4(_m, p, q);

		if (bFirstEase) {
			bFirstEase = false;
			easedPosition = p;
			easedQuaternion = q;
		}
		
		easedPosition = glm::lerp(p, easedPosition, easingParam);
		easedQuaternion = glm::slerp(q, easedQuaternion, easingParam);

	}; break;
	case FRAME_FILTER_KALMAN: default: {

		// decompose the matrix and update the filters
		glm::vec3 p;
		glm::quat q;
		decomposeMat4x4(_m, p, q);

		kalmanPosition.update(p);
		kalmanOrientation.update(q);

	}; break;
	}
}

// --------------------------------------------------
glm::mat4x4 ofxFilter::getFrame() {
	switch (mode) {
	case FRAME_FILTER_NONE: {

		return lastFrame;

	}; break;
	case FRAME_FILTER_EASING: {

		glm::mat4x4 outMat;
		composeMat4x4(easedPosition, easedQuaternion, outMat);
		return outMat;

	}; break;
	case FRAME_FILTER_KALMAN: default: {

		glm::quat q = quatConvert(kalmanOrientation.getPrediction());
		glm::vec3 p = kalmanPosition.getPrediction();

		glm::mat4x4 outMat;
		composeMat4x4(p, q, outMat);
		return outMat;

	}; break;
	}
}

// --------------------------------------------------
void ofxFilter::setMode(ofxFilterMode _mode) {
	mode = _mode;
}

// --------------------------------------------------
void ofxFilter::setParamsKalman(float smoothness, float rapidness, bool bUseAccel) {
	kalmanPosition.init(smoothness, rapidness, bUseAccel);
	kalmanOrientation.init(smoothness, rapidness, bUseAccel);
	// Do we also need to input the last measurement again?

}

// --------------------------------------------------
void ofxFilter::addScalar(float _s) {
	addFrame(glm::translate(glm::vec3(_s, 0, 0)));
}

// --------------------------------------------------
float ofxFilter::getScalar() {
	return getTranslation(getFrame()).x;
}

// --------------------------------------------------
void ofxFilter::setParamsEasing(float _easingParam) {
	easingParam = _easingParam;
}

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------