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
	string modeNames[] = { "None", "Kalman" }; //, "Easing" };
	RUI_SHARE_ENUM_PARAM_WCN("FG" + name + "- Mode", settings.mode, FILTER_NONE, NUM_FILTER_MODES-1, modeNames);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Kalman Smoothness", settings.kalmanSmoothness, 0, 12);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Kalman Rapidness", settings.kalmanRapidness, 0, 12);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Kalman Use Accel", settings.bKalmanUseAccel);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Kalman Use Jerk", settings.bKalmanUseJerk);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Easing Param", settings.easingParam, 0, 1);

	string predNames[] = { "None", "Kalman", "Acc" };
	RUI_SHARE_ENUM_PARAM_WCN("FG" + name + "- Pred Mode", settings.predMode, FILTER_PRED_NONE, NUM_FILTER_PRED_MODES-1, predNames);
	//RUI_SHARE_PARAM_WCN("FG" + name + "- Pred Delay", settings.predDelay, 0, 20);

	string postNames[] = { "None", "Easing" };
	RUI_SHARE_ENUM_PARAM_WCN("FG" + name + "- Post Mode", settings.postMode, FILTER_POST_NONE, NUM_FILTER_POST_MODES-1, postNames);
	RUI_SHARE_PARAM_WCN("FG" + name + "- Post Easing Param", settings.postEasingParam, 0, 1);

}

// --------------------------------------------------
void ofxFilterGroup::setup() {

	updateAllParams();

	ofAddListener(RUI_GET_OF_EVENT(), this, &ofxFilterGroup::paramsUpdated);

}

// --------------------------------------------------
ofxFilter* ofxFilterGroup::getFilter(string key, bool bCreateIfNone) {

	if (filters.find(key) == filters.end()) {
		if (!bCreateIfNone) return NULL;

		// couldn't find it; create a new one
		ofxFilter* f = new ofxFilter();
		// Update params
		f->setParams(settings);
		f->setup();
		// Save the filter
		filters[key] = f;
	}
	return filters[key];
}

// --------------------------------------------------
bool ofxFilterGroup::filterExists(string key) {

	return filters.find(key) != filters.end();
}

// --------------------------------------------------
void ofxFilterGroup::paramsUpdated(RemoteUIServerCallBackArg& arg) {
	if (arg.action != CLIENT_UPDATED_PARAM) return;
	if (arg.group.compare(ruiGroupName) != 0) return;

	updateAllParams();
}

// --------------------------------------------------
void ofxFilterGroup::updateAllParams() {

	for (auto it = filters.begin(); it != filters.end(); it++) {
		it->second->setParams(settings);
	}
}

// --------------------------------------------------


// --------------------------------------------------
