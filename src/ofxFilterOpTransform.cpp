#include "ofxFilterOpTransform.h"

// --------------------------------------------------
void ofxFilterOpTransformSettings::setupParams() {

	// Don't create a new group, just add on params
    RUI_SHARE_PARAM_WCN(getIDA() + "- Translate X", translation.x, -1000000, 1000000);
    RUI_SHARE_PARAM_WCN(getIDA() + "- Translate Y", translation.y, -1000000, 1000000);
    RUI_SHARE_PARAM_WCN(getIDA() + "- Translate Z", translation.z, -1000000, 1000000);

    RUI_SHARE_PARAM_WCN(getIDA() + "- Rotate X Pitch", rotation.x, -360, 360); // [-90, 90]
    RUI_SHARE_PARAM_WCN(getIDA() + "- Rotate Y Yaw", rotation.y, -360, 360); // [-180, 180]
    RUI_SHARE_PARAM_WCN(getIDA() + "- Rotate Z Roll", rotation.z, -360, 360); // [-180, 180]

    RUI_SHARE_PARAM_WCN(getIDA() + "- Scale X", scale.x, 0, 1000000); // negative?
    RUI_SHARE_PARAM_WCN(getIDA() + "- Scale Y", scale.y, 0, 1000000);
    RUI_SHARE_PARAM_WCN(getIDA() + "- Scale Z", scale.z, 0, 1000000);
}

// --------------------------------------------------
void ofxFilterOpTransform::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpTransform::_process(ofxFilterData& data) {

    // If we don't have valid data, return
    if (!data.bValid) return;
    
    // Get the settings for this operator
    ofxFilterOpTransformSettings* s = static_cast<ofxFilterOpTransformSettings*>(settings);

    // Retrive the transformation matrices
    auto _t = glm::translate(s->translation);
    auto _r = glm::eulerAngleYXZ(
        float(s->rotation.y * DEG_TO_RAD),
        float(s->rotation.x * DEG_TO_RAD),
        float(s->rotation.z * DEG_TO_RAD));
    auto _s = glm::scale(s->scale);

    // Apply them in SRT order
    data.m = _t * (_r * (_s * data.m));
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
