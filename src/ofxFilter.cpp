#include "ofxFilter.h"

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

glm::vec3 getTranslation(glm::mat4x4& a) {
	return glm::vec3(a[3][0], a[3][1], a[3][2]);
}

glm::vec3 getXAxis(glm::mat4x4& a) {
	return glm::vec3(a[0][0], a[0][1], a[0][2]);
}

glm::vec3 getYAxis(glm::mat4x4& a) {
	return glm::vec3(a[1][0], a[1][1], a[1][2]);
}

glm::vec3 getZAxis(glm::mat4x4& a) {
	return glm::vec3(a[2][0], a[2][1], a[2][2]);
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
void ofxFilter::add(glm::mat4x4& _m) {
	lastFrame = _m;
	switch (mode) {
	case FILTER_NONE: {

		// nothing to do

	}; break;
	case FILTER_EASING: {

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
	case FILTER_KALMAN: default: {

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
	case FILTER_NONE: {

		return lastFrame;

	}; break;
	case FILTER_EASING: {

		glm::mat4x4 outMat;
		composeMat4x4(easedPosition, easedQuaternion, outMat);
		return outMat;

	}; break;
	case FILTER_KALMAN: default: {

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
void ofxFilter::setParamsKalman(float smoothness, float rapidness, bool bUseAccel, bool bUseJerk) {
	kalmanPosition.init(smoothness, rapidness, bUseAccel, bUseJerk);
	kalmanOrientation.init(smoothness, rapidness, bUseAccel, bUseJerk);
	// Do we also need to input the last measurement again?

}

// --------------------------------------------------
void ofxFilter::add(float _s) {
	glm::mat4 m = glm::translate(glm::vec3(_s, 0, 0));
	add(m);
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
void ofxFilter::add(glm::vec3 _p) {
	glm::mat4 m = glm::translate(_p);
	add(m);
}

// --------------------------------------------------
glm::vec3 ofxFilter::getPosition() {
	return getTranslation(getFrame());
}

// --------------------------------------------------
void ofxFilter::add(glm::vec2 _p) {
	glm::mat4 m = glm::translate(glm::vec3(_p.x, _p.y, 0));
	add(m);
}

// --------------------------------------------------
glm::vec2 ofxFilter::getPosition2D() {
	auto p = getTranslation(getFrame());
	return glm::vec2(p.x, p.y);
}

// --------------------------------------------------
void ofxFilter::add() {

	// Progress forward one time step
	kalmanPosition.predict();
	kalmanOrientation.predict();
}

// --------------------------------------------------
void ofxFilter::add(glm::quat quaternion) {

	glm::mat4x4 rotMat = glm::toMat4(quaternion);
	add(rotMat);
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