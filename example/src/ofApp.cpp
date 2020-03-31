#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    // Set the frame rate of all filters
    ofxFilterUnits::one()->setFPS(240);
    
    ofSetVerticalSync(false);
    
	// Load the RUI database from file
	RUI_SET_CONFIGS_DIR("configs");
	RUI_SETUP();
	RUI_LOAD_FROM_XML();

	// Setup the filter group
    filters.setup("mouse", "easing");
//    filters.getFilter("myMouse");
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Update the frame rate of the app if it doesn't match the filters
    if (int(round(ofGetTargetFrameRate())) != int(round(ofxFilterUnits::one()->fps()))) {
        ofSetFrameRate(int(round(ofxFilterUnits::one()->fps())));
    }

    bool bOneDim = false;
    
    bool bPred = true;
    if (bFilterActive || bAuto) {

        if (bAuto) {
            // Add an automatic circle measurement
            if (bMousePressed && (ofGetFrameNum()%int(ofxFilterUnits::one()->fps()) > 12)) {
                glm::vec2 position;
                position.x = cos(ofGetElapsedTimef()*2.0)/2.5 + 0.5;
                position.y = sin(ofGetElapsedTimef()*2.0)/2.5 + 0.5;
                filters.getFilter("myMouse")->process(position);
                bPred = false;
            }
            else {
                filters.getFilter("myMouse")->process();
                bPred = true;
            }
            
        } else {
        
            // Add a measurement by mouse
            if (bMousePressed) {
                float rad = 5;
                glm::vec2 position = glm::vec2(ofGetMouseX() + ofRandom(-rad, rad), ofGetMouseY() + ofRandom(-rad, rad));
                position /= glm::vec2(ofGetHeight(), ofGetHeight());
                if (bOneDim) position.y = 0.0;
                filters.getFilter("myMouse")->process(position);
                bPred = false;
            }
            else {
                filters.getFilter("myMouse")->process();
                bPred = true;
            }
        }
        

		// Make a prediction
		bool bValid = filters.getFilter("myMouse")->isDataValid();
		if ((bValid && !bLastValid) || lines.empty()) {
			lines.push_back(ofPolyline());
            preds.push_back(vector<bool>());
		}
		bLastValid = bValid;
        if (bValid) {
            glm::vec2 pred = filters.getFilter("myMouse")->getPosition2D();
            if (bOneDim) pred.y += fmod(float(ofGetElapsedTimef()),10.0)/10.0;
            pred *= glm::vec2(ofGetHeight(), ofGetHeight());
            lines.back().addVertex(pred.x, pred.y, 0);
            preds.back().push_back(bPred);
        }
	}

	if (!lines.empty() && lines.back().size() > nMaxMeasurements) {
		lines.back().getVertices().erase(lines.back().getVertices().begin(), lines.back().getVertices().begin() + (lines.back().getVertices().size() - nMaxMeasurements));
        preds.back().erase(preds.back().begin(), preds.back().begin() + (preds.back().size() - nMaxMeasurements));
		lines.back().flagHasChanged();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(255, 200, 200);
    
    if (bAuto) {
        ofSetColor(0, 50);
        ofDrawCircle(ofGetHeight()/2, ofGetHeight()/2, ofGetHeight()/2.5);
    }

	// Draw a circle at the tip of the mouse
	ofSetColor(0);
	if (bMousePressed) ofFill();
	else ofNoFill();
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);

	ofFill();
	ofSetLineWidth(1);
	
	for (int i = 0; i < lines.size(); i++) {
        ofSetColor(255, 0, 0);
		lines[i].draw();
        
        for (int j = 0; j < lines[i].getVertices().size(); j++) {
            
            if (preds[i][j]) {
                ofSetColor(0, 255, 0, 180);
                ofDrawCircle(lines[i].getVertices()[j], 3);
            } else {
                ofSetColor(0, 0, 255, 180);
                ofDrawCircle(lines[i].getVertices()[j], 3);
            }
            
        }
	}

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') bAuto = !bAuto;

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	bMousePressed = true;
	bFilterActive = true;


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bMousePressed = false;

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
