#include "ofxFilterOpAxes.h"

// --------------------------------------------------
void ofxFilterOpAxesSettings::setupParams() {

	// Don't create a new group, just add on params
    RUI_SHARE_ENUM_PARAM_WCN(getIDA() + "- Src Handedness", srcHandedness, HandednessMode::RIGHT_HANDED, HandednessMode::LEFT_HANDED, getHandednessModes());
    RUI_SHARE_ENUM_PARAM_WCN(getIDA() + "- Src Up Vector", srcUpVector, UpVectorMode::UP_X, UpVectorMode::UP_Z, getUpVectorModes());

    RUI_SHARE_PARAM_WCN(getIDA() + "- Convert Handedness", bConvertHandedness);
    RUI_SHARE_ENUM_PARAM_WCN(getIDA() + "- Dst Handedness", dstHandedness, HandednessMode::RIGHT_HANDED, HandednessMode::LEFT_HANDED, getHandednessModes());
    
    RUI_SHARE_PARAM_WCN(getIDA() + "- Convert Up Vector", bConvertUpVector);
    RUI_SHARE_ENUM_PARAM_WCN(getIDA() + "- Dst Up Vector", dstUpVector, UpVectorMode::UP_X, UpVectorMode::UP_Z, getUpVectorModes());

    RUI_SHARE_PARAM_WCN(getIDA() + "- Num Up Vec Rotations", nUpVectorRotations, -12, 12);
}

// --------------------------------------------------
void ofxFilterOpAxes::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

}

// --------------------------------------------------
void ofxFilterOpAxes::_process(ofxFilterData& data) {

    // If we don't have valid data, return
    if (!data.bValid) return;
    
    // Get the settings for this operator
    ofxFilterOpAxesSettings* s = static_cast<ofxFilterOpAxesSettings*>(settings);

    // Adjust the handedness first
    if (s->bConvertHandedness) {
        if (s->srcHandedness != s->dstHandedness) {
            glm::mat4 T = glm::mat4(
                int(s->srcUpVector == ofxFilterOpAxesSettings::UP_X), int(s->srcUpVector == ofxFilterOpAxesSettings::UP_Z), int(s->srcUpVector == ofxFilterOpAxesSettings::UP_Y), 0,
                int(s->srcUpVector == ofxFilterOpAxesSettings::UP_Z), int(s->srcUpVector == ofxFilterOpAxesSettings::UP_Y), int(s->srcUpVector == ofxFilterOpAxesSettings::UP_X), 0,
                int(s->srcUpVector == ofxFilterOpAxesSettings::UP_Y), int(s->srcUpVector == ofxFilterOpAxesSettings::UP_X), int(s->srcUpVector == ofxFilterOpAxesSettings::UP_Z), 0,
                0, 0, 0, 1);
            data.m = T * data.m;
        }
    }

    // Adjust the up vector
    if (s->bConvertUpVector) {
        if (s->srcUpVector != s->dstUpVector) {
            int dist = (int(s->dstUpVector) - int(s->srcUpVector) + 3) % 3;
            glm::mat4 T = glm::mat4(
                int(dist == 0), int(dist == 1), int(dist == 2), 0,
                int(dist == 2), int(dist == 0), int(dist == 1), 0,
                int(dist == 1), int(dist == 2), int(dist == 0), 0,
                0, 0, 0, 1
            );
            data.m = T * data.m;
        }
    }

    // Add rotations around the up vector
    if (s->nUpVectorRotations != 0) {
        int upv = int(s->bConvertUpVector ? s->dstUpVector : s->srcUpVector);
        int h = int(s->bConvertHandedness ? s->dstHandedness : s->srcHandedness);
        glm::mat4 _T = glm::mat4(
            int(upv == 0),  -int(upv == 2), int(upv == 1),  0,
            int(upv == 2),  int(upv == 1),  -int(upv == 0), 0,
            -int(upv == 1), int(upv == 0),  int(upv == 2),  0,
            0, 0, 0, 1
        );
        int nRot = (s->nUpVectorRotations + 4 * (abs(s->nUpVectorRotations) / 4 + 1)) % 4;
        glm::mat4 T = glm::mat4();
        for (int i = 0; i < nRot; i++) T = _T * T;
        data.m = T * data.m;
    }
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
