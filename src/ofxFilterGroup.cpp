#include "ofxFilterGroup.h"

// --------------------------------------------------
ofxFilterGroup::ofxFilterGroup() {

}

// --------------------------------------------------
ofxFilterGroup::~ofxFilterGroup() {
    
}

// --------------------------------------------------
void ofxFilterGroup::setup(string _name, string _opList) {
    
    // Setup filter units if not yet setup
    ofxFilterUnits::one()->setup();

	// Create a group for all of the parameters
	name = _name;
	ruiGroupName = "ofxFilterGroup - " + name;
	string logPrefix = "FG_" + name;
	opList = _opList;
	RUI_NEW_GROUP(ruiGroupName);
	RUI_SHARE_PARAM_WCN("FG_" + name + "- " + "opList", opList);

	// Now that the operator list is loaded, create a list of settings
	// and initialize the parameters of these settings.
	vector<string> opListParsed = ofSplitString(opList, ",");
	opSettings.clear();
	int depth = -1;
	for (int i = 0; i < opListParsed.size(); i++) {
		string type = ofToLower(opListParsed[i]);
		depth++;

		ofxFilterOpSettings* settings;
		if (type == "none") {
			settings = new ofxFilterOpSettings();
		}
		else if (type == "easing") {
			settings = new ofxFilterOpEasingSettings();
		}
		else if (type == "kalman") {
			settings = new ofxFilterOpKalmanSettings();
		}
		else if (type == "add-rate") {
			settings = new ofxFilterOpAddRateSettings();
		}
		else if (type == "continuity") {
			// This must follow an "add-rate"
			if (i == 0 || ofToLower(opListParsed[i - 1]) != "add-rate") {
				ofLogWarning("ofxFilterGroup") << "Continuity operators must follow Add-Rate";
			}
			settings = new ofxFilterOpContinuitySettings();
		}
		else {
			ofLogError("ofxFilter") << "Operator type \"" << type << "\" is not valid.";
			depth--;
			continue;
		}
		settings->setup(logPrefix, depth);
		opSettings.push_back(settings);
	}

	ofAddListener(RUI_GET_OF_EVENT(), this, &ofxFilterGroup::paramsUpdated);

}

// --------------------------------------------------
ofxFilter* ofxFilterGroup::getFilter(string key, bool bCreateIfNone) {
	bool bExists = filterExists(key);
	if (!bExists && !bCreateIfNone) return NULL;

	// Create a filter if it doesn't exist
	if (!bExists) {
		ofxFilter* filter = new ofxFilter();
		filter->setup(opSettings);
		filters[key] = filter;
	}

	// Save that this filter was called for
	filtersCalled[key] = true;

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

	// TODO?
}

// --------------------------------------------------
void ofxFilterGroup::processRemaining() {

	for (auto& it : filters) {
		if (filtersCalled[it.first]) { // was called
			filtersCalled[it.first] = false;
		}
		else { // was not called, so process it without data
			it.second->process();
		}
	}
}

// --------------------------------------------------
