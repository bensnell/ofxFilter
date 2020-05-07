#include "ofxFilterOpKalman.h"

// --------------------------------------------------
void ofxFilterOpKalmanSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getIDA() + "- Smoothness", smoothness, -10, 10);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Rapidness", rapidness, -10, 10);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Use Accel", bUseAccel);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Use Jerk", bUseJerk);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Predict Empty", bPredictEmpty);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Max Empty Pred", nMaxEmptyPredictions, -1, 100);
	RUI_SHARE_PARAM_WCN(getIDA() + "- Reset After Empty", bResetAfterEmpty);

}

// --------------------------------------------------
void ofxFilterOpKalman::initFilters() {

	ofxFilterOpKalmanSettings* s = static_cast<ofxFilterOpKalmanSettings*>(settings);

	kt.init(s->getCalcSmoothness(), s->getCalcRapidness(), s->bUseAccel, s->bUseJerk);
	kr.init(s->getCalcSmoothness(), s->getCalcRapidness(), s->bUseAccel, s->bUseJerk);
	ks.init(s->getCalcSmoothness(), s->getCalcRapidness(), s->bUseAccel, s->bUseJerk);
}

// --------------------------------------------------
void ofxFilterOpKalman::setup(ofxFilterOpSettings* _settings) {

	ofxFilterOp::setup(_settings);

	initFilters();
}

// --------------------------------------------------
void ofxFilterOpKalman::_process(ofxFilterData& data) {

	ofxFilterOpKalmanSettings* s = static_cast<ofxFilterOpKalmanSettings*>(settings);
	
	// If we don't have valid data and aren't predicting empty, then don't output data
	if (!data.bValid && (!s->bPredictEmpty || (s->nMaxEmptyPredictions != -1 && nEmptyPred > s->nMaxEmptyPredictions))) {
		if (s->bResetAfterEmpty && !bInitFilters) initFilters();
		bInitFilters = true;
		return;
	}
	bInitFilters = false;

	if (!data.bValid && s->bPredictEmpty) {
		// Progress all predictions forward one time step without a measurement.
		// These calls take a lot of time, so only do them if these measures are being tracked.
		if (data.validMeasures[0]) kt.predict();
		if (data.validMeasures[1]) kr.predict();
		if (data.validMeasures[2]) ks.predict();

		nEmptyPred++;
	}
	else {
		// Add a new measurement to all filters
		if (data.validMeasures[0]) kt.update(data.translation());
		if (data.validMeasures[1]) kr.update(data.rotation());
		if (data.validMeasures[2]) ks.update(data.scale());

		nEmptyPred = 0;
	}

	// Set the new data
	data.bValid = true;
	ofQuaternion q;
	if (data.validMeasures[1]) q = kr.getPrediction();
	data.set(
		data.validMeasures[0] ? kt.getPrediction() : glm::vec3(),
		data.validMeasures[1] ? quatConvert(q) : glm::quat(),
		data.validMeasures[2] ? ks.getPrediction() : glm::vec3(1,1,1));
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
