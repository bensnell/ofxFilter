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

	// Now that the operator list is loaded, create a valid list of all settings.
	vector<string> opListParsed = ofSplitString(opList, ",");
	opSettings.clear();
	int depth = -1;
	bool bAge = false;
	for (int i = 0; i < opListParsed.size(); i++) {
		string type = ofToLower(opListParsed[i]);
		depth++;

		ofxFilterOpSettings* settings;
		if (type == "none") {
			settings = new ofxFilterOpSettings();
			opSettings.push_back(settings);
		}
		else if (type == "easing") {
			settings = new ofxFilterOpEasingSettings();
			opSettings.push_back(settings);
		}
		else if (type == "kalman") {
			settings = new ofxFilterOpKalmanSettings();
			opSettings.push_back(settings);
		}
		else if (type == "add-rate") {
			settings = new ofxFilterOpAddRateSettings();
			opSettings.push_back(settings);
		}
		else if (type == "continuity") {
			// This must follow an "add-rate"
			if (i == 0 || ofToLower(opListParsed[i - 1]) != "add-rate") {
				ofLogWarning("ofxFilterGroup") << "Continuity operators must follow Add-Rate";
			}
			settings = new ofxFilterOpContinuitySettings();
			opSettings.push_back(settings);
		}
		else if (type == "axes") {
			settings = new ofxFilterOpAxesSettings();
			opSettings.push_back(settings);
		} 
		else if (type == "age") {
			if (bAge) {	// There can only be one "age" op
				ofLogError("ofxFilterGroup") << "Only one Age op can be supplied. It will automatically wrap the stack.";
				continue;
			}
			// The "age" op must be placed at the beginning and the end.
			settings = new ofxFilterOpAgeSettings();
			opSettings.insert(opSettings.begin(), settings);
			bAge = true;
		}
		else {
			if (type.empty()) ofLogNotice("ofxFilter") << "Operator type is empty.";
			else ofLogError("ofxFilter") << "Operator type \"" << type << "\" is not valid.";
			depth--;
			continue;
		}
	}

	// Set the depths of these operators
	for (int i = 0; i < opSettings.size(); i++) {
		opSettings[i]->addDepth(i);
		if (opSettings[i]->getType() == "age") opSettings[i]->addDepth(opSettings.size());
	}

	// Setup a parameter to enable each operator
	for (int i = 0; i < opSettings.size(); i++) {
		RUI_SHARE_PARAM_WCN(logPrefix + "_" + opSettings[i]->getAbbr() + "_" + opSettings[i]->getDepth() + "- Enabled", opSettings[i]->bEnabled);
	}

	// Setup operator-specific parameters
	for (int i = 0; i < opSettings.size(); i++) {
		opSettings[i]->setup(logPrefix);
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

	// Process all filters that have not been processed yet.
	// This will call process on empty (invalid) data as input.
	for (auto& it : filters) {
		if (!it.second->wasProcessed()) it.second->process();
		it.second->resetProcessFlag();
	}
}

// --------------------------------------------------
long ofxFilterGroup::maxLifespan() {
	// Calculate and return the max lifespan.
	// -1 (infinite) lifespans will be passed through.
	long lifespan = 1;
	long out = 1;
	for (int i = 0; i < opSettings.size(); i++) {
		lifespan = opSettings[i]->maxLifespan();
		if (lifespan < 0) return -1;
		out = max(out, lifespan);
	}
	return out;
}

// --------------------------------------------------
void ofxFilterGroup::removeUnused() {

	// Get the maximum lifespan across all operators
	long maxLS = maxLifespan();
	// If the lifespan is infinite, do not proceed.
	if (maxLS < 0) return;

	// Delete any filters that have exceeded their lifespan
	auto it = filters.begin();
	while (it != filters.end()) {
		if (it->second->getNumInvalidOutputs() > maxLS) {
			it = filters.erase(it);
		}
		else {
			++it;
		}
	}
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
