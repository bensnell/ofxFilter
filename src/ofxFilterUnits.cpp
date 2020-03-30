#include "ofxFilter.h"

ofxFilterUnits* ofxFilterUnits::_instance = NULL;

// --------------------------------------------------
ofxFilterUnits* ofxFilterUnits::one() {
    if (!_instance) {
        _instance = new ofxFilterUnits();
    }
    return _instance;
}

// --------------------------------------------------
void ofxFilterUnits::setup() {
    if (bSetup) return;
    
    RUI_NEW_GROUP("ofxFilterUnits");
    RUI_SHARE_PARAM_WCN("FUnits- FPS", _fps, 0, 1000);
    
    
    
    bSetup = true;
}

// --------------------------------------------------
float ofxFilterUnits::convertEaseParam(float param, float refFPS) {
    
    return exp(log(param)*refFPS/fps());
}

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

// --------------------------------------------------

