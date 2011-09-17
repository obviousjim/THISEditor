#include "testApp.h"
#include "ofFileUtils.h"
//--------------------------------------------------------------
void testApp::setup(){

	ofEnableAlphaBlending();

	timeline = new THISTimeline();
	timeline->setup();


    timeline->position = ofPoint(0, 3*720/4*.75 + 20);
    timeline->uiposition = ofPoint(1280, 0);

	outputImage = NULL;

	ofSetFullscreen(true);

	ofDisableDataPath();
}

//--------------------------------------------------------------
void testApp::update(){
	timeline->width = ofGetWidth();
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(20,20,20);
	ofEnableAlphaBlending();
	
    if(timeline->allSequencesLoaded()){

        ofTexture* sourceAStart = timeline->getCurrentPreviewStartSourceAFrame();
        ofTexture* sourceAEnd = timeline->getCurrentPreviewEndSourceAFrame();
        ofTexture* sourceBStart = timeline->getCurrentPreviewStartSourceBFrame();
        ofTexture* sourceBEnd = timeline->getCurrentPreviewEndSourceBFrame();

        ofTexture* previewStartDistortionFrame = timeline->getCurrentPreviewStartDistortionFrame();
        ofTexture* previewEndDistortionFrame = timeline->getCurrentPreviewEndDistortionFrame();

        ofTexture* previewDistortionFrame = timeline->getCurrentPreviewBlendedDistortionFrame();
        ofTexture* previewOutputFrame = timeline->getCurrentPreviewOutputFrame();


        float scaleFactor = .75;
        if(sourceAStart != NULL){
            sourceAStart->draw(0,0,
                               sourceAStart->getWidth()*scaleFactor, sourceAStart->getHeight()*scaleFactor);
        }
        if(sourceAEnd != NULL){
            sourceAEnd->draw(sourceAEnd->getWidth()*scaleFactor, 0,
                             sourceAEnd->getWidth()*scaleFactor, sourceAEnd->getHeight()*scaleFactor);
        }
        if(sourceBStart != NULL){
            sourceBStart->draw(0, sourceBStart->getHeight()*scaleFactor,
                               sourceBStart->getWidth()*scaleFactor, sourceBStart->getHeight()*scaleFactor);
        }
        if(sourceBEnd != NULL){
            sourceBEnd->draw(sourceBEnd->getWidth()*scaleFactor, sourceBEnd->getHeight()*scaleFactor,
                             sourceBEnd->getWidth()*scaleFactor, sourceBEnd->getHeight()*scaleFactor);
        }

        if(previewStartDistortionFrame != NULL){
            previewStartDistortionFrame->draw(0, 2*previewStartDistortionFrame->getHeight()*scaleFactor,
                                         previewStartDistortionFrame->getWidth()*scaleFactor, previewStartDistortionFrame->getHeight()*scaleFactor);
        }

        if(previewEndDistortionFrame != NULL){
            previewEndDistortionFrame->draw(previewEndDistortionFrame->getWidth()*scaleFactor, 2*previewEndDistortionFrame->getHeight()*scaleFactor,
                                              previewEndDistortionFrame->getWidth()*scaleFactor, previewEndDistortionFrame->getHeight()*scaleFactor);
        }

        if(previewDistortionFrame != NULL){
            previewDistortionFrame->draw(2*previewDistortionFrame->getWidth()*scaleFactor, 2*previewDistortionFrame->getHeight()*scaleFactor,
                                         previewDistortionFrame->getWidth()*scaleFactor, previewDistortionFrame->getHeight()*scaleFactor);
        }
        if( previewOutputFrame != NULL){
            previewOutputFrame->draw(2*previewOutputFrame->getWidth()*scaleFactor,0, previewOutputFrame->getWidth()*scaleFactor*2, previewOutputFrame->getHeight()*scaleFactor*2);
        }
        if(outputImage){
            outputImage->draw(200, 400);
        }

    }

    timeline->draw();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if(key == 'f'){
		ofToggleFullscreen();
	}
	if(key == ' '){
		timeline->togglePlayback();
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

