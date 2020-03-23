#include "ofxFilterOpKalman.h"

// --------------------------------------------------
void ofxFilterOpKalmanSettings::setupParams() {

	// Don't create a new group, just add on params

	RUI_SHARE_PARAM_WCN(getID() + "- Smoothness", smoothness, -10, 10);
	RUI_SHARE_PARAM_WCN(getID() + "- Rapidness", rapidness, -10, 10);
	RUI_SHARE_PARAM_WCN(getID() + "- Use Accel", bUseAccel);
	RUI_SHARE_PARAM_WCN(getID() + "- Use Jerk", bUseJerk);
	RUI_SHARE_PARAM_WCN(getID() + "- Predict Empty", bPredictEmpty);
	RUI_SHARE_PARAM_WCN(getID() + "- Max Empty Pred", nMaxEmptyPredictions, -1, 100);
	RUI_SHARE_PARAM_WCN(getID() + "- Reset After Empty", bResetAfterEmpty);

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
void ofxFilterOpKalman::process(ofxFilterData& data) {

	ofxFilterOpKalmanSettings* s = static_cast<ofxFilterOpKalmanSettings*>(settings);
	
	// If we don't have valid data and aren't predicting empty, then don't output data
	if (!data.bValid && (!s->bPredictEmpty || (s->nMaxEmptyPredictions != -1 && nEmptyPred > s->nMaxEmptyPredictions))) {
		if (s->bResetAfterEmpty) initFilters();
		return;
	}

	if (!data.bValid && s->bPredictEmpty) {
		// Progress all predictions forward one time step without a measurement
		kt.predict();
		kr.predict();
		ks.predict();

		nEmptyPred++;
	}
	else {
		// Add a new measurement to all filters
		kt.update(data.getTranslation());
		kr.update(data.getRotation());
		ks.update(data.getScale());

		nEmptyPred = 0;
	}

	// Set the new data
	data.bValid = true;
	data.set(kt.getPrediction(), quatConvert(kr.getPrediction()), ks.getPrediction());
}

// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------


// --------------------------------------------------
