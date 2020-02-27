//
//  ofxFilterGroup.cpp
//  g5_002
//
//  Created by Ben Snell on 1/17/19.
//

#include "ofxFilterGroup.h"

// --------------------------------------------------
ofxFilterGroup::ofxFilterGroup() {

}

// --------------------------------------------------
ofxFilterGroup::~ofxFilterGroup() {
    
}

// --------------------------------------------------
void ofxFilterGroup::setupParams(string name) {
	groupName = name;

	ruiGroupName = "Filter Group- " + groupName;
	RUI_NEW_GROUP(ruiGroupName);
	string modeNames[] = { "None", "Kalman", "Easing" };
	RUI_SHARE_ENUM_PARAM_WCN("FG" + name + "- Mode", mode, FILTER_NONE, FILTER_EASING, modeNames);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Smooth Exponent", smoothnessExp, 0, 12);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Rapid Exponent", rapidnessExp, 0, 12);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Use Accel", bUseAccel);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Easing Param", easingParam, 0, 1);

}

// --------------------------------------------------
void ofxFilterGroup::setup() {

	updateAllParams();

	ofAddListener(RUI_GET_OF_EVENT(), this, &ofxFilterGroup::paramsUpdated);

}

// --------------------------------------------------
glm::mat4x4 ofxFilterGroup::applyFilter(string _key, glm::mat4x4 _frame) {
	ofxFilter* f = getFilter(_key);
	f->add(_frame);
	return f->getFrame();
}

// --------------------------------------------------
float ofxFilterGroup::applyFilter(string _key, float _scalar) {
	ofxFilter* f = getFilter(_key);
	f->add(_scalar);
	return f->getScalar();
}

// --------------------------------------------------
ofxFilter* ofxFilterGroup::getFilter(string _key) {

	if (filters.find(_key) == filters.end()) {
		// couldn't find it; create a new one
		ofxFilter* f = new ofxFilter();
		// Update params
		updateParams(f);
		// Save the filter
		filters[_key] = f;
	}
	return filters[_key];
}

// --------------------------------------------------
void ofxFilterGroup::paramsUpdated(RemoteUIServerCallBackArg& arg) {
	if (arg.action != CLIENT_UPDATED_PARAM) return;
	if (arg.group.compare(ruiGroupName) != 0) return;

	updateAllParams();
}

// --------------------------------------------------
void ofxFilterGroup::updateAllParams() {

	smoothness = powf(0.1, smoothnessExp);
	rapidness = powf(0.1, rapidnessExp);

	for (auto it = filters.begin(); it != filters.end(); it++) {
		updateParams(it->second);
	}
}

// --------------------------------------------------
void ofxFilterGroup::updateParams(ofxFilter* filter) {
	filter->setMode(mode);
	filter->setParamsKalman(smoothness, rapidness, bUseAccel);
	filter->setParamsEasing(easingParam);
}

// --------------------------------------------------


// --------------------------------------------------
