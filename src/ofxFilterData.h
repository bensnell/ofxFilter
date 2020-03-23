#pragma once

#include "ofMain.h"
#include "ofxFilterUtils.h"

// transformation matrix rate
class mat4rate {
public:

	vector<bool> b;			// valid rate?
	vector<glm::vec3> t;	// translation rate
	vector<glm::vec3> r;	// rotation rate (euler angles)
	vector<glm::vec3> s;	// scale rate
	
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

	int size() { return b.size(); }


};

class ofxFilterData {
public:

	// Is this data valid (or is it empty?)
	bool bValid = true;

	// Frame data
	glm::mat4 m;

	// Rate Parameters
	mat4rate r;
	void updateRate(int nElapsedFrames = 1);
	void applyFriction(float friction); // one frame friction
	// TODO: interleave friction with prediction?
	bool predictFromRate(int nFrames = 1); // one frame prediction
	bool setFromRate(); // set the transf matrix from the rate


	// Set the transformation matrix
	void set(glm::vec3& translation, glm::quat& rotation, glm::vec3 scale) {
		m = glm::translate(translation)* glm::toMat4(rotation)* glm::scale(scale);
	}

	// Get values from the transformation matrix
	glm::vec3 getTranslation() { return { m[3][0], m[3][1], m[3][2] }; }
	glm::quat getRotation() { return glm::quat_cast(m); }
	glm::vec3 getScale() {
		return glm::vec3(
			glm::l2Norm(glm::vec3( m[0][0], m[0][1], m[0][2] )),
			glm::l2Norm(glm::vec3( m[1][0], m[1][1], m[1][2] )),
			glm::l2Norm(glm::vec3( m[2][0], m[2][1], m[2][2] )));
	}

	// Lerp to another data object
	void lerp(ofxFilterData& to, float amt);

protected:

};
