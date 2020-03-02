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

	lastMeasurement = glm::translate(glm::vec3(0, 0, 0));

	kalmanPosition.init(0.001, 0.01);
	kalmanOrientation.init(0.001, 0.01);
    
}

// --------------------------------------------------
ofxFilter::~ofxFilter() {
    
}

// --------------------------------------------------
void ofxFilter::add(glm::mat4x4& _m) {

	// Save this measurement
	lastMeasurement = _m;

	// Add a measurement
	switch (settings.mode) {
	case FILTER_NONE: {

		// nothing to do

	}; break;
	//case FILTER_EASING: {

	//	glm::vec3 p;
	//	glm::quat q;
	//	decomposeMat4x4(_m, p, q);

	//	if (bFirstEase) {
	//		bFirstEase = false;
	//		easedPosition = p;
	//		easedQuaternion = q;
	//	}
	//	
	//	easedPosition = glm::lerp(p, easedPosition, settings.easingParam);
	//	easedQuaternion = glm::slerp(q, easedQuaternion, settings.easingParam);

	//}; break;
	case FILTER_KALMAN: default: {

		// decompose the matrix and update the filters
		glm::vec3 p;
		glm::quat q;
		decomposeMat4x4(_m, p, q);

		kalmanPosition.update(p);
		kalmanOrientation.update(q);

	}; break;
	}

	// Mark that we have just received a measurement
	nEmptyMeasurements = 0;

	// Save the current measurement
	thisFrame = getFilteredFrame();
}

// --------------------------------------------------
glm::mat4x4 ofxFilter::getFilteredFrame() {
	switch (settings.mode) {
	case FILTER_NONE: {

		return lastMeasurement;

	}; break;
	//case FILTER_EASING: {

	//	glm::mat4x4 outMat;
	//	composeMat4x4(easedPosition, easedQuaternion, outMat);
	//	return outMat;

	//}; break;
	case FILTER_KALMAN: default: {

		//glm::mat3 all = kalmanPosition.getPredictionAll();
		//cout << "Pos:\t" << all[0][0] << "\t" << all[0][1] << "\t" << all[0][2] << endl;
		//cout << "Vel:\t" << all[1][0] << "\t" << all[1][1] << "\t" << all[1][2] << endl;
		//cout << "Acc:\t" << all[2][0] << "\t" << all[2][1] << "\t" << all[2][2] << endl;

		glm::quat q = quatConvert(kalmanOrientation.getPrediction());
		glm::vec3 p = kalmanPosition.getPrediction();

		glm::mat4x4 outMat;
		composeMat4x4(p, q, outMat);
		return outMat;

	}; break;
	}
}

// --------------------------------------------------
void ofxFilter::setParams(ofxFilterSettings _settings) {

	// Save the settings
	settings = _settings;

}

// --------------------------------------------------
void ofxFilter::setup() {

	// Initialize all necessary things
	initialize();

}

// --------------------------------------------------
void ofxFilter::initialize() {

	// Kalman filters require initialization
	cout << settings.getCalcKalmanRapidness() << "\t" << settings.getCalcKalmanSmoothness() << endl;
	kalmanPosition.init(
		settings.getCalcKalmanSmoothness(),
		settings.getCalcKalmanRapidness(),
		settings.bKalmanUseAccel,
		settings.bKalmanUseJerk);
	kalmanOrientation.init(
		settings.getCalcKalmanSmoothness(),
		settings.getCalcKalmanRapidness(),
		settings.bKalmanUseAccel,
		settings.bKalmanUseJerk);

}

// --------------------------------------------------
void ofxFilter::add(float _s) {
	glm::mat4 m = glm::translate(glm::vec3(_s, 0, 0));
	add(m);
}

// --------------------------------------------------
void ofxFilter::add(glm::vec3 _p) {
	glm::mat4 m = glm::translate(_p);
	add(m);
}

// --------------------------------------------------
void ofxFilter::add(glm::vec2 _p) {
	glm::mat4 m = glm::translate(glm::vec3(_p.x, _p.y, 0));
	add(m);
}

// --------------------------------------------------
void ofxFilter::add(glm::quat quaternion) {

	glm::mat4x4 rotMat = glm::toMat4(quaternion);
	add(rotMat);
}

// --------------------------------------------------
void ofxFilter::add() {

	// Increment the number of empty measurements
	nEmptyMeasurements++;

	// Predict, if it is on
	switch (settings.predMode) {
	case FILTER_PRED_NONE: {

		// Do not make predictions

	}; break;
	case FILTER_PRED_KALMAN: {

		// Predict using the regular kalman filter (predict one time step without an 
		// actual measurement).
		// This works well for a few frames, but quickly diverges after that.

		kalmanPosition.predict();
		kalmanOrientation.predict();

	}; break;
	case FILTER_PRED_ACC: default: {

		// TODO: What motion parameters can be used for "FILTER_NONE"?

		// Use the last observed parameters to begin predicting





	}; break;
	}


}

// --------------------------------------------------
glm::mat4x4 ofxFilter::getFrame() {
	return thisFrame;
}

// --------------------------------------------------
float ofxFilter::getScalar() {
	return getTranslation(getFrame()).x;
}

// --------------------------------------------------
glm::vec3 ofxFilter::getPosition() {
	return getTranslation(getFrame());
}

// --------------------------------------------------
glm::vec2 ofxFilter::getPosition2D() {
	auto p = getTranslation(getFrame());
	return glm::vec2(p.x, p.y);
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

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------
