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

		transformOrder = SRT;
	};
	~ofxFilterOpTransformSettings() {};

	void setupParams();

	glm::vec3 translation;
	glm::vec3 rotation; // euler
	glm::vec3 scale;

	enum TRANSFORM_ORDER
	{
		SRT = 0,
		STR,
		RST,
		RTS,
		TSR,
		TRS
	};
	vector<string> getTransformOrders()
	{
		return { "SRT", "STR", "RST", "RTS", "TSR", "TRS" };
	}
	TRANSFORM_ORDER transformOrder;
	
};

class ofxFilterOpTransform : public ofxFilterOp {
public:

	// Setup this operator
	void setup(ofxFilterOpSettings* _settings);

protected:

	// Apply this operator to data and get transformed data as output
	void _process(ofxFilterData& data);


};
