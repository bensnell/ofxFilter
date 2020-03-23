#include "ofxFilterData.h"

// --------------------------------------------------
void mat4rate::forward(glm::mat4 _m, int nElapsedFrames) {

	// Calculate the current euler angle (warping to correct dimension)
	glm::vec3 euler = glm::eulerAngles(getRotation(_m));
	if (size() > 0 && b[0]) {
		glm::vec3 eulerPrev = r[0];
		for (int i = 0; i < 3; i++) {
			float rev = floorf((eulerPrev[i] + 180) / 360.f) * 360;
			euler[i] += rev;
			if (euler[i] < -90 + rev && eulerPrev[i] > 90 + rev) euler[i] += 360;
			else if (euler[i] > 90 + rev && eulerPrev[i] < -90 + rev) euler[i] -= 360;
		}
	}

	// Update rates
	glm::vec3 _t = getTranslation(_m);
	glm::vec3 _r = euler;
	glm::vec3 _s = getScale(_m);
	glm::vec3 tmp;
	for (int i = 0; i < size(); i++) {

		// normalize changes that occur over multiple frames
		float div = (i == 1) ? float(nElapsedFrames) : 1.0;

		if (i == 0) {
			swap(_t, t[i]);
			swap(_r, r[i]);
			swap(_s, s[i]);
		}
		else {
			tmp = t[i];
			t[i] = (t[i - 1] - _t) / div;
			_t = tmp;

			tmp = r[i];
			r[i] = (r[i - 1] - _r) / div;
			_r = tmp;

			tmp = s[i];
			s[i] = (s[i - 1] - _s) / div;
			_s = tmp;
		}

		if (!b[i]) {
			b[i] = true;
			break;
		}
	}
}

// --------------------------------------------------
void mat4rate::applyFriction(float friction) {

	// Don't apply friction to lowest-order parameters (skip 0)
	for (int i = 1; i < size(); i++) {
		
		// Don't apply friction to parameters that don't exist yet
		if (!b[i]) break;
		
		t[i] *= friction;
		r[i] *= friction;
		s[i] *= friction;
	}
}

// --------------------------------------------------
void mat4rate::backward(int nFrames) {
	
	// If there is no valid rate information, we cannot predict
	if (size() <= 1 || !b[1]) return;

	// Backpropogate the rates
	for (int f = 0; f < nFrames; f++) {
		for (int i = size() - 2; i >= 0; i--) {
			if (!b[i]) break;

			t[i] += t[i + 1];
			r[i] += r[i + 1];
			s[i] += s[i + 1];
		}
	}
}

// --------------------------------------------------
void ofxFilterData::updateRate(int nElapsedFrames) {

	r.forward(m, nElapsedFrames);
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

	glm::vec3 _t = translation() * (1.0 - amt) + to.translation() * amt;
	glm::quat _r = glm::slerp(rotation(), to.rotation(), amt);
	glm::vec3 _s = scale() * (1.0 - amt) + to.scale() * amt;
	
	set(_t, _r, _s);
}

// --------------------------------------------------
glm::vec3 ofxFilterData::translation() {
	return getTranslation(m);
}

// --------------------------------------------------
glm::quat ofxFilterData::rotation() {
	return getRotation(m);
}

// --------------------------------------------------
glm::vec3 ofxFilterData::scale() {
	return getScale(m);
}

// --------------------------------------------------
void ofxFilterData::reconcile(ofxFilterData& a, ReconciliationMode mode) {

	switch (mode) {
	case ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_FRAME: {

		// The frame is copied, along with the first rate (which is the also the
		// frame). This leaves the rates as-is. If the next velocity differs
		// significantly from the current one, then this may create high
		// accelerations (whiplash).

		bValid = a.bValid;
		m = a.m;
		if (r.size() > 0) {		// TODO (?): check for validity?
			r.b[0] = a.r.b[0];
			r.t[0] = a.r.t[0];
			r.r[0] = a.r.r[0];
			r.s[0] = a.r.s[0];
		}

	}; break;
	case ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_FRAME_AND_UPDATE_RATE: {

		// ===== UPDATE USING FRAME =====
		// Motion parameters are updated using this frame. This frame becomes
		// the new frame. If the observed and predicted higher order (2+) rates
		// differ significantly, there will be whiplash. 

		bValid = a.bValid;
		m = a.m;
		updateRate();

	}; break;
	case ofxFilterData::ReconciliationMode::OFXFILTERDATA_RECONCILE_COPY_ALL: default: {

		// ===== ALL =====
		// The frame and rate are copied from the observation
		// (This may result in uneven steps, since there is no reconciliation
		// between predictions and observations during this linked state. 
		// However, the rate information is reliable, since it comes 
		// directly from observations

		bValid = a.bValid;
		m = a.m;
		r = a.r;

	}; break;
	}

}

// --------------------------------------------------

// --------------------------------------------------
