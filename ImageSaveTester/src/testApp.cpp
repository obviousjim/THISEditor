#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	rgbImage.loadImage("testImage.png");
	
	bwImage.clone(rgbImage);
	bwImage.setImageType(OF_IMAGE_GRAYSCALE);
	bwImage.saveImage("bwOutput.png");
	reloadedImage.loadImage("bwOutput.png");
	
	cout << "reloaded type is " << reloadedImage.type << " " << OF_IMAGE_GRAYSCALE << endl;
	
	dw = rgbImage.getWidth()/4;
	dh = rgbImage.getHeight()/4;
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	rgbImage.draw(0, 0, dw, dh);
	bwImage.draw(dw,0, dw, dh);
	reloadedImage.draw(dw*2, 0, dw, dh);
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

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