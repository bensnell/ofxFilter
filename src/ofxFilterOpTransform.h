#pragma once

#include "ofMain.h"
#include "ofxRemoteUIServer.h"
#include "ofxFilterUtils.h"
#include "ofxFilterData.h"
#include "ofxFilterOp.h"

class ofxFilterOpTransformSettings : public ofxFilterOpSettings {
public:

	ofxFilterOpTransformSettings() {
		type = "transform";
		abbr = "TFM";

		translation = glm::vec3(0, 0, 0);
		rotation = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	};
	~ofxFilterOpTransformSettings() {};

	void setupParams();

	glm::vec3 translation;
	glm::vec3 rotation; // euler
	glm::vec3 scale;
};

class ofxFilterOpTransform : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);


};
