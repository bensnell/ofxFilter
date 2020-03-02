#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetFrameRate(60);


	ofxFilterSettings settings;
	settings.kalmanSmoothness = 2;
	settings.kalmanRapidness = -1;
	settings.bKalmanUseAccel = true;
	settings.bKalmanUseJerk = false;
	settings.predMode = ofxFilterPredMode::FILTER_PRED_ACC;
	settings.postMode = ofxFilterPostMode::FILTER_POST_EASING;

	filter.setParams(settings);
	filter.setup();

}

//--------------------------------------------------------------
void ofApp::update(){

	if (bFilterActive) {

		// Add a measurement
		if (bMousePressed) {
			float rad = 5;
			filter.add(glm::vec2(ofGetMouseX()+ofRandom(-rad,rad), ofGetMouseY()+ofRandom(-rad,rad)));
		}
		else {
			filter.add();
		}

		// Make a prediction
		glm::vec2 pred = filter.getPosition2D();
		line.addVertex(pred.x, pred.y, 0);
	}

	if (line.size() > 200) {
		line.getVertices().erase(line.getVertices().begin(), line.getVertices().begin() + (line.getVertices().size() - 200));
		line.flagHasChanged();
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
	line.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
