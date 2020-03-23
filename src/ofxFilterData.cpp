#include "ofxFilterData.h"

// --------------------------------------------------
void ofxFilterData::updateRate(int nElapsedFrames) {

	// Calculate the current euler angle (warping to correct dimension)
	glm::vec3 euler = glm::eulerAngles(getRotation());
	if (r.size() > 0 && r.b[0]) {
		glm::vec3 eulerPrev = r.r[0];
		for (int i = 0; i < 3; i++) {
			float rev = floorf((eulerPrev[i] + 180) / 360.f) * 360;
			euler[i] += rev;
			if (euler[i] < -90 + rev && eulerPrev[i] > 90 + rev) euler[i] += 360;
			else if (euler[i] > 90 + rev && eulerPrev[i] < -90 + rev) euler[i] -= 360;
		}
	}

	// Update rates
	glm::vec3 _t = getTranslation();
	glm::vec3 _r = euler;
	glm::vec3 _s = getScale();
	glm::vec3 tmp;
	for (int i = 0; i < r.size(); i++) {

		// normalize changes that occur over multiple frames
		float div = (i == 1) ? float(nElapsedFrames) : 1.0;

		if (i == 0) {
			swap(_t, r.t[i]);
			swap(_r, r.r[i]);
			swap(_s, r.s[i]);
		}
		else {
			tmp = r.t[i];
			r.t[i] = (r.t[i - 1] - _t) / div;
			_t = tmp;

			tmp = r.r[i];
			r.r[i] = (r.r[i - 1] - _r) / div;
			_r = tmp;

			tmp = r.s[i];
			r.s[i] = (r.s[i - 1] - _s) / div;
			_s = tmp;
		}

		if (!r.b[i]) {
			r.b[i] = true;
			break;
		}
	}
}

// --------------------------------------------------
void ofxFilterData::applyFriction(float friction) {

	// Don't apply friction to lowest-order parameters (skip 0)
	for (int i = 1; i < r.size(); i++) {
		
		// Don't apply friction to parameters that don't exist yet
		if (!r.b[i]) break;
		
		r.t[i] *= friction;
		r.r[i] *= friction;
		r.s[i] *= friction;
	}
}

// --------------------------------------------------
bool ofxFilterData::predictFromRate(int nFrames) {
	
	// If there is no valid rate information, we cannot predict
	if (r.size() <= 1 || !r.b[1]) return false;

	// Backpropogate the rates
	for (int f = 0; f < nFrames; f++) {
		for (int i = r.size() - 2; i >= 0; i--) {
			if (!r.b[i]) break;

			r.t[i] += r.t[i + 1];
			r.r[i] += r.r[i + 1];
			r.s[i] += r.s[i + 1];
		}
	}
	return true;
}

// --------------------------------------------------
bool ofxFilterData::setFromRate() {

	if (r.size() < 1 || !r.b[0]) return false;

	ofQuaternion q;
	q.set(0, 0, 0, 1);
	q.makeRotate(r.r[0].x, glm::vec3(1, 0, 0), r.r[0].z, glm::vec3(0, 0, 1), r.r[0].y, glm::vec3(0, 1, 0));
	
	set(r.t[0], quatConvert(q), r.s[0]);
	
	return true;
}


// --------------------------------------------------
void ofxFilterData::lerp(ofxFilterData& to, float amt) {

	glm::vec3 _t = getTranslation() * (1.0 - amt) + to.getTranslation() * amt;
	glm::quat _r = glm::slerp(getRotation(), to.getRotation(), amt);
	glm::vec3 _s = getScale() * (1.0 - amt) + to.getScale() * amt;
	
	set(_t, _r, _s);
}

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------
