#include "ofxFilterData.h"

// --------------------------------------------------
void mat4rate::forward(glm::mat4 _m, int nElapsedFrames, float easeParam) {

	// Calculate the current euler angle (warping to correct dimension)
	glm::vec3 euler = glm::eulerAngles(getRotation(_m));
	if (size() > 0 && b[0]) {
		euler = getEulerWarped(euler, r[0]);
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

			// TODO:
			// Should rates be eased incrementally, so higher order rates change slower? (mix now)
			// Or should rates change at the same time, so all rates change the same amount? 
			//	(mix applied after ALL updates)

			tmp = t[i];
			t[i] = glm::mix((t[i - 1] - _t) / div, t[i], easeParam);
			_t = tmp;

			tmp = r[i];
			r[i] = glm::mix((r[i - 1] - _r) / div, r[i], easeParam);
			_r = tmp;

			tmp = s[i];
			s[i] = glm::mix((s[i - 1] - _s) / div, s[i], easeParam);
			_s = tmp;
		}

		if (!b[i]) {
			b[i] = true;
			break;
		}
	}
}

// --------------------------------------------------
void mat4rate::applyFriction(float friction, float ratePower) {

	// Don't apply friction to lowest-order parameters (skip 0)
	for (int i = 1; i < size(); i++) {
		
		// Don't apply friction to parameters that don't exist yet
		if (!b[i]) break;
		
		for (int j = 0; j < 3; j++) {
			(*this)[j][i] *= pow(friction, pow(ratePower, i - 1));
		}
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

	// TODO: Should rotations be normalized after back propogation?
}

// --------------------------------------------------
bool ofxFilterData::converge(ofxFilterData& to, ConvergenceParams& p) {

	// TODO: Check to make sure rates are valid (and there are enough of them)
	if (r.size() < 3 || to.r.size() < 3 || !r.isOrderValid(2) || !to.r.isOrderValid(1)) return false;

	for (int i = 0; i < 3; i++) {

		// If FROM and TO frames are the same, then skip
		if (m == to.m) continue;

		// First, find the vector to the target
		glm::vec3 heading;
		if (i == 1) { // rotation
			heading = getEulerWarped(to.r[i][0], r[i][0]) - r[i][0];
		}
		else {
			heading = to.r[i][0] - r[i][0];
		}

		// If the heading is too small, then skip
		float epsilon = glm::l2Norm(heading);
		if (epsilon < pow(10, -p.epsilonPower)) continue;
        
        ofLogNotice("FD") << "\t\tHeading\t\t\t" << heading;

		// Next, determine approximately how long it would take to approach the target.
		float k0 = ofMap(glm::dot(glm::normalize(r[i][1]), glm::normalize(to.r[i][1])), -1, 1, 2, 1, true);
		float k1 = ofMap(glm::dot(glm::normalize(r[i][1]), glm::normalize(heading)), -1, 1, 2, 1, true);
		float timeToTarget = (k0 * k1 * (glm::l2Norm(heading) / glm::l2Norm(r[i][1]))) / p.frameRate;
        ofLogNotice("FD") << "\t\tTime to Target\t" << timeToTarget;

		// Determine the target speed
		float maxSpeedPerFrame = p.maxSpeed[i] / p.frameRate;
		float paramToMaxSpeed = ofMap(timeToTarget / p.approachTime, p.approachBuffer, 1, 0, 1, true);
		float targetSpeed = ofLerp(glm::l2Norm(to.r[i][1]), maxSpeedPerFrame, paramToMaxSpeed);
        ofLogNotice("FD") << "\t\tTarget Speed\t" << targetSpeed << "\tto: " << glm::l2Norm(to.r[i][1]) << "\t maxPerFrame: " << maxSpeedPerFrame << "\tparam: " << paramToMaxSpeed;
        
		// Determine the target velocity
		glm::vec3 targetVel = glm::normalize(heading)* targetSpeed;
        ofLogNotice("FD") << "\t\tTarget Vel\t\t" << targetVel;

		// Determine the target acceleration
		glm::vec3 targetAcc = targetVel - r[i][1];
        ofLogNotice("FD") << "\t\tTarget Acc\t\t" << targetAcc;

		// Calculate the required jerk
		glm::vec3 targetJerk = targetAcc - r[i][2];
		float maxAccStepPerFrame = p.maxSpeed[i] / pow(p.frameRate, p.accStepPower);
		float jerkMagnitude = min(glm::l2Norm(targetJerk), maxAccStepPerFrame);
		glm::vec3 jerk = glm::normalize(targetJerk) * jerkMagnitude;

		// Calculate the new (adjusted) accleration that would be required to
		// produce the motion we desire (convergence on the target frame) and set it.
		r[i][2] = r[i][2] + jerk;
	}

	// TODO: Should they converge evenly?

	return true;
}

// --------------------------------------------------
void ofxFilterData::updateRateFromFrame(int nElapsedFrames, float easeParam) {

	r.forward(m, nElapsedFrames, easeParam);
}

// --------------------------------------------------
bool ofxFilterData::setFrameFromRate() {

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
		updateRateFromFrame();

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
bool ofxFilterData::similar(ofxFilterData& a, SimilarityParams& p) {

	// Are this data and the other data similar?

	float mix = 0;
	int nOrders = min(min(p.nRates, r.size()), a.r.size());
	for (int order = 0; order < nOrders; order++) {

		// Calculate the differences (l2 distances)
		glm::vec3 diff;
		if (order == 0) {
			// Look at the frame

			// Calculate the difference in translation.
			diff[0] = glm::l2Norm(translation(), a.translation());

			// Calculate the difference in rotation.
			// This should be done with angle() and axisAngle() of quats, but for
			// ease, we will use euler angles.
			// Warp the euler angles so they traverse the shortest path.
			glm::vec3 euler = glm::eulerAngles(rotation());
			glm::vec3 refEuler = glm::eulerAngles(a.rotation());
			euler = getEulerWarped(euler, refEuler);
			// Calc the difference
			diff[1] = glm::l2Norm(euler, refEuler);

			// Calculate the difference in scale.
			diff[2] = glm::l2Norm(scale(), a.scale());
		}
		else {
			// Look at the rates

			// Only proceed if valid
			if (!r.b[0] || !a.r.b[0]) continue;

			diff[0] = glm::l2Norm(r.t[order], a.r.t[order]);
			diff[1] = glm::l2Norm(r.r[order], a.r.r[order]);
			diff[2] = glm::l2Norm(r.s[order], a.r.s[order]);
		}

		// Apply the thresholds and sum them up
		for (int i = 0; i < 3; i++) {
            mix += diff[i] == 0.0 ? 0.0 : (log(diff[i] / (p.thresh[i] * pow(p.rateThreshMult, order))) * p.mix[i] * pow(2.0, -order * p.rateWeight));
		}
	}

	// The frames are similar if the mixture is less or equal to 0
	return mix <= 0.0;
}

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------
