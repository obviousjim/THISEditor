#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	lastChoice = "select a file";
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofPushMatrix();
	ofScale(3, 3, 3);
	ofDrawBitmapString(lastChoice, ofPoint(30, 30) );
	ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofFileDialogResult f = ofSystemLoadDialog("Open a file dude", false);
	if(f.bSuccess){
		lastChoice = "you picked " + f.getPath();
	}
	else {
		lastChoice = "you canceled the file";
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}