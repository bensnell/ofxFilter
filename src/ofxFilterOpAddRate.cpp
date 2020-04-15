#include "ofxFilterOpAddRate.h"

// --------------------------------------------------
void ofxFilterOpAddRateSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getID() + "- Order", order, 1, 4);
	RUI_SHARE_PARAM_WCN(getID() + "- Frames to Reset", nFramesToResetRate, 0, 120);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Fast Ease", forwardParams.fastEaseParam, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Slow Ease", forwardParams.slowEaseParam, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Default Ease", forwardParams.defaultEaseParam, 0, 1);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Ease Power", forwardParams.easeParamRatePower, 0, 10);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Max Trans Speed", forwardParams.maxSpeed[0], 0, 20);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Max Rot Speed", forwardParams.maxSpeed[1], 0, 360);
    RUI_SHARE_PARAM_WCN(getID() + "- RateF Max Scale Speed", forwardParams.maxSpeed[2], 0, 20);
    
    
    
}

// --------------------------------------------------
void ofxFilterOpAddRate::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpAddRate::process(ofxFilterData& data) {

    ofxFilterOpAddRateSettings* s = static_cast<ofxFilterOpAddRateSettings*>(settings);

    // Initialize the rate parameters if the order is not the same
    if ((data.r.size() != s->order) ||
        (data.bValid && !bLastDataValid && (s->nFramesToResetRate >= 0) && (nFramesSinceValidData > s->nFramesToResetRate))) {
        lastValidData.r.init(s->order);
    }
    
    if (data.bValid) {
        
        // Set the new data
        lastValidData.set(data.m);
        // Update the rate
        lastValidData.updateRateFromFrame(nFramesSinceValidData, s->forwardParams);
        // And output this data
        data = lastValidData;
        
        // Reset the frame counter, since this data is valid
        nFramesSinceValidData = 0;
        
    } else {
        // No valid data, so don't change output or save anything
    }
    
    // Increment the number of elapsed frames
    nFramesSinceValidData++;
    
    // Save whether this was valid
    bLastDataValid = data.bValid;
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
