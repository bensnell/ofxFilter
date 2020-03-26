#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);

	// Load the RUI database from file
	RUI_SET_CONFIGS_DIR("configs");
	RUI_SETUP();
	RUI_LOAD_FROM_XML();

	// Setup the filter group
	filters.setup("mouse", "easing");
	filters.getFilter("myMouse");
    
}

//--------------------------------------------------------------
void ofApp::update(){

	if (bFilterActive) {

		// Add a measurement
		if (bMousePressed) {
			float rad = 5;
			glm::vec2 position = glm::vec2(ofGetMouseX() + ofRandom(-rad, rad), ofGetMouseY() + ofRandom(-rad, rad));
			position /= glm::vec2(ofGetHeight(), ofGetHeight());
			filters.getFilter("myMouse")->process(position);
		}
		else {
			filters.getFilter("myMouse")->process();
		}

		// Make a prediction
		bool bValid = filters.getFilter("myMouse")->isDataValid();
		if ((bValid && !bLastValid) || lines.empty()) {
			lines.push_back(ofPolyline());
		}
		bLastValid = bValid;
		glm::vec2 pred = filters.getFilter("myMouse")->getPosition2D();
		pred *= glm::vec2(ofGetHeight(), ofGetHeight());
		lines.back().addVertex(pred.x, pred.y, 0);
	}

	if (!lines.empty() && lines.back().size() > 200) {
        ofLogNotice("ofApp") << lines.back().getVertices().front();
		lines.back().getVertices().erase(lines.back().getVertices().begin(), lines.back().getVertices().begin() + (lines.back().getVertices().size() - 200));
		lines.back().flagHasChanged();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(255, 200, 200);

	// Draw a circle at the tip of the mouse
	ofSetColor(0);
	if (bMousePressed) ofFill();
	else ofNoFill();
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);

	ofFill();
	ofSetLineWidth(2);
	ofSetColor(255, 0, 0);
	for (int i = 0; i < lines.size(); i++) {
		lines[i].draw();
	}

	ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == ' ') {
        ofLogNotice("ofApp") << "FLAG";
    }

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
