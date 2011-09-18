/*
 *  THISTimeline.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#include "THISTimeline.h"
//#include "ofxFileDialogCocoa.h"

#define MAX_BLENDED_DISTORTION 25

bool blendedframesort(BlendedDistortionFrame* a, BlendedDistortionFrame* b)
{
    return a->lastUsedTime < b->lastUsedTime;
}

THISTimeline::THISTimeline()
{
    playheadPosition = 0;
    playheadFloatPosition = 0;
    zoomedPlayheadPosition = 0;
	zoomedPlayheadFloatPosition = 0;

	rollingOverPlayhead = false;
	distortionPreviewPixelsChar = NULL;
    maxDistortionWidth = 100;

	previewStartSourceAFrame = new ofTexture();
	previewEndSourceAFrame = new ofTexture();

	previewStartSourceBFrame = new ofTexture();
	previewEndSourceBFrame = new ofTexture();

	previewStartDistortionFrame = new ofTexture();
	previewEndDistortionFrame = new ofTexture();

	previewBlendedDistortionFrame = new ofTexture();
	previewOutputFrame = new ofTexture();

	isPlayingBack = false;

	exporter = NULL;
	exporting = false;
}

THISTimeline::~THISTimeline()
{

}

void THISTimeline::setup()
{
	sourceA = new THISSequence();
	sourceA->setDrawWidth(1024);

	sourceB = new THISSequence();
	sourceB->setDrawWidth(1024);

	distortion = new THISSequence();
	distortion->setImageType(OF_IMAGE_GRAYSCALE);
	distortion->setDrawWidth(1024);

	output = new THISOutputTimeline();
	output->inputTimeline = this;

	maxwidthkeys = new THISKeyframeEditor();
	minwidthkeys = new THISKeyframeEditor();
	sourcekeys = new THISKeyframeEditor();
	blendkeys = new THISKeyframeEditor();
	zoomer = new THISTimelineZoom();

	//TODO: make many and multithreaded...
	exporter = new THISExporter();
	exporter->inputTimeline = this;
	exporter->outputTimeline = output;

	newCompButton = new ofxMSAInteractiveObjectWithDelegate();
	newCompButton->setup();
	newCompButton->setDelegate(this);
	newCompButton->disableAppEvents();

	loadCompButton = new ofxMSAInteractiveObjectWithDelegate();
	loadCompButton->setup();
	loadCompButton->setDelegate(this);
	loadCompButton->disableAppEvents();

	/*
	loadSourceBButton = new ofxMSAInteractiveObjectWithDelegate();
	loadSourceBButton->setup();
	loadSourceBButton->setDelegate(this);
	loadSourceBButton->disableAppEvents();

	loadDistortionButton = new ofxMSAInteractiveObjectWithDelegate();
	loadDistortionButton->setup();
	loadDistortionButton->setDelegate(this);
	loadDistortionButton->disableAppEvents();

	setOutputDirectoryButton = new ofxMSAInteractiveObjectWithDelegate();
	setOutputDirectoryButton->setup();
	setOutputDirectoryButton->setDelegate(this);
	setOutputDirectoryButton->disableAppEvents();
	 */
	
	playheadBar = new ofxMSAInteractiveObjectWithDelegate();
    playheadBar->setup();
    playheadBar->setDelegate(this);
    playheadBar->disableAppEvents();

	exportCurrentViewButton = new ofxMSAInteractiveObjectWithDelegate();
	exportCurrentViewButton->setup();
	exportCurrentViewButton->setDelegate(this);
    exportCurrentViewButton->disableAppEvents();

	exportEntireSequenceButton = new ofxMSAInteractiveObjectWithDelegate();
	exportEntireSequenceButton->setup();
	exportEntireSequenceButton->setDelegate(this);
    exportEntireSequenceButton->disableAppEvents();

	cancelExportButton = new ofxMSAInteractiveObjectWithDelegate();
	cancelExportButton->setup();
	cancelExportButton->setDelegate(this);
    cancelExportButton->disableAppEvents();

	string defaultComp = "";
	if(defaults.loadFile("defaults.xml")){
		defaultComp = defaults.getValue("defaults:comp", "");
	}

	if(defaultComp == ""){
		ofLog(OF_LOG_ERROR, "THISTimeline -- No defaults comp, creating one.");
		ofEnableDataPath();
		if(!ofDirectory::doesDirectoryExist("defaultcomp")){
			ofDirectory::createDirectory("defaultcomp", true);
		}
		defaultComp = ofToDataPath("deafultcomp/", true);
		
		cout << "DEFAULT COMP IS " << defaultComp << endl;
		ofDisableDataPath();

	}
	
	if(!loadComposition(defaultComp)){
		newComposition();
	}
}

bool THISTimeline::loadComposition(string compFolder)
{
	if(compFolder == "" || !ofDirectory::doesDirectoryExist(compFolder, false)){
		ofLog(OF_LOG_ERROR, "THISTimeline -- comp '" + compFolder + "' failed to load");
		return false;
	}

	setWorkingFolder(compFolder);
	
    if(settings.loadFile(settingsFileName)){
		return loadComposition(settings);
    }
	else{
		ofSystemAlertDialog("THISTimeline -- LOAD COMP -- Failed to load settings file '" + settingsFileName + "'.");
		exporter->pathPrefix = compFolder;
		return false;
	}
	return true;
}

void THISTimeline::setWorkingFolder(string compFolder)
{
	currentCompFolder = compFolder;
	
	ofEnableDataPath();
	defaults.setValue("defaults:comp", compFolder);
	defaults.saveFile("defaults.xml");
	ofDisableDataPath();
	
	maxwidthkeys->setXMLFileName(compFolder+"/max_width_keys.xml");
	maxwidthkeys->clear();
	maxwidthkeys->loadFromXML();
	
	minwidthkeys->setXMLFileName(compFolder+"/min_width_keys.xml");
	minwidthkeys->loadFromXML();
	
	sourcekeys->setXMLFileName(compFolder+"/source_keys.xml");
	sourcekeys->loadFromXML();
	
	blendkeys->setXMLFileName(compFolder+"/blend_keys.xml");
	blendkeys->loadFromXML();
	
	zoomer->setXmlFileName(compFolder+"/zoom_window.xml");
	zoomer->loadFromXML();
	
	settingsFileName = compFolder+"/sequence_settings.xml";	
}

bool THISTimeline::loadComposition(ofxXmlSettings compSettings)
{
	settings = compSettings;
	
	string sourceSequenceADirectory = settings.getValue("settings:source_directory_a", "");
	string sourceSequenceBDirectory = settings.getValue("settings:source_directory_b", "");
	string disortionSequenceDirectory = settings.getValue("settings:distortion_directory", "");
	string outputSequenceDirectory = settings.getValue("settings:output_directory", "");
	if(sourceSequenceADirectory != "" && ofDirectory::doesDirectoryExist(sourceSequenceADirectory)){
		sourceA->loadSequence(sourceSequenceADirectory);
	}
	else {
		ofSystemAlertDialog("THISTimeline -- LOAD COMP -- Sequence A failed to load from directory " + sourceSequenceADirectory);
		return false;
	}
	
	if(sourceSequenceBDirectory != "" && ofDirectory::doesDirectoryExist(sourceSequenceBDirectory)){
		sourceB->loadSequence(sourceSequenceBDirectory);
	}
	else{
		ofSystemAlertDialog("THISTimeline -- LOAD COMP -- Sequence B failed to load from directory: " + sourceSequenceBDirectory);
		return false;
	}
	
	if(disortionSequenceDirectory != "" && ofDirectory::doesDirectoryExist(disortionSequenceDirectory)){
		distortion->loadSequence(disortionSequenceDirectory);
		clearDistortionCaches();
		createDistortionCaches();
	}
	else{
		ofSystemAlertDialog("THISTimeline -- LOAD COMP -- Distortion Sequence failed to load from directory: " + disortionSequenceDirectory);
		return false;
	}
	
	if(outputSequenceDirectory != "" && ofDirectory::doesDirectoryExist(outputSequenceDirectory)){
		exporter->pathPrefix = outputSequenceDirectory;
	}
	else {
		ofSystemAlertDialog("THISTimeline -- LOAD COMP -- Output directory '" + outputSequenceDirectory + "' not found");
		return false;
	}
	
	playheadPosition = settings.getValue("settings:playhead", 0.0);
	return true;
}

#define FRAME_TICKER_HEIGHT 20
#define SOURCE_PREVIEW_HEIGHT 75
#define KEYFRAME_HEIGHT (150/3) //for laptop testing
//#define KEYFRAME_HEIGHT 150 //for final output
#define GRAPH_HEIGHT 75
#define SWITCHER_HEIGHT 30
#define BUTTON_WIDTH 25
#define ELEMENT_SPACER 15
#define BIG_BUTTON_WIDTH 700
#define BIG_BUTTON_HEIGHT 40

void THISTimeline::draw()
{

	//update keyframers
	minwidthkeys->setZoomBounds(zoomer->getViewRange());
	maxwidthkeys->setZoomBounds(zoomer->getViewRange());
	sourcekeys->setZoomBounds(zoomer->getViewRange());
	blendkeys->setZoomBounds(zoomer->getViewRange());

	if(!zoomer->isActive()){
		sourceA->setDrawRange(zoomer->getViewRange());
		sourceB->setDrawRange(zoomer->getViewRange());
		distortion->setDrawRange(zoomer->getViewRange());
	}

	if(exporting && !exporter->isExporting()){
		exporting = false;
		minwidthkeys->enable();
		maxwidthkeys->enable();
		sourcekeys->enable();
		blendkeys->enable();
		zoomer->enable();
	}

	if(isPlayingBack){
		zoomedPlayheadFloatPosition = ofMap(ofGetElapsedTimef(), playbackStartTime, playbackStartTime+(getLastFrameInView()-getFirstFrameInView())/25., 0, zoomer->getViewRange().max-zoomer->getViewRange().min, false);
		zoomedPlayheadFloatPosition = zoomer->getViewRange().min + fmod(zoomedPlayheadFloatPosition, zoomer->getViewRange().max-zoomer->getViewRange().min);
	}

	//-------- POSITION ALL ELEMENTS

	//-------- COMP ELEMENTS
	newCompButton->setPos(uiposition.x, uiposition.y);
	newCompButton->setSize(BIG_BUTTON_WIDTH, BIG_BUTTON_HEIGHT);

	loadCompButton->setPos(uiposition.x, uiposition.y+BIG_BUTTON_HEIGHT+ELEMENT_SPACER);
    loadCompButton->setSize(BIG_BUTTON_WIDTH, BIG_BUTTON_HEIGHT);

//	loadSourceBButton->setPos(uiposition.x, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*2);
//    loadSourceBButton->setSize(BIG_BUTTON_WIDTH, BIG_BUTTON_HEIGHT);
//
//	loadDistortionButton->setPos(uiposition.x, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*3);
//    loadDistortionButton->setSize(BIG_BUTTON_WIDTH, BIG_BUTTON_HEIGHT);
//
//	setOutputDirectoryButton->setPos(uiposition.x, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*4);
//    setOutputDirectoryButton->setSize(BIG_BUTTON_WIDTH, BIG_BUTTON_HEIGHT);


	//------------- EXPORT BUTTONS
	exportCurrentViewButton->setPos(uiposition.x, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*5);
	exportCurrentViewButton->setSize(BIG_BUTTON_WIDTH/3.0, BIG_BUTTON_HEIGHT);

	exportEntireSequenceButton->setPos(uiposition.x+BIG_BUTTON_WIDTH/3.0, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*5);
	exportEntireSequenceButton->setSize(BIG_BUTTON_WIDTH/3.0, BIG_BUTTON_HEIGHT);

	cancelExportButton->setPos(uiposition.x+2*BIG_BUTTON_WIDTH/3.0, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*5);
	cancelExportButton->setSize(BIG_BUTTON_WIDTH/3.0, BIG_BUTTON_HEIGHT);


	//---------- TIMELINE ELEMENTS
	THISSequence* loadedReferenceSource = NULL;
	if(sourceA->isLoaded()) loadedReferenceSource = sourceA;
	else if(sourceB->isLoaded()) loadedReferenceSource = sourceB;
	else if(distortion->isLoaded()) loadedReferenceSource = distortion;

	float sourcePreviewHeight = SOURCE_PREVIEW_HEIGHT;
	if(loadedReferenceSource != NULL){
		sourcePreviewHeight = loadedReferenceSource->heightPerFrame;
	}
						//frame ticker		  //3 previews			  //5 keyframes		  //A-B Switch		//zoomer
	float totalHeight = FRAME_TICKER_HEIGHT + sourcePreviewHeight*3 + KEYFRAME_HEIGHT*4 + GRAPH_HEIGHT + SWITCHER_HEIGHT + FRAME_TICKER_HEIGHT + ELEMENT_SPACER*4;
    float totalWidth = width-BUTTON_WIDTH*2;
    playheadBar->setPos(position.x+BUTTON_WIDTH, position.y);
    playheadBar->setSize(totalWidth, totalHeight);



	//---------- KEYFRAME POSITIONS
	float keyframeStartY = position.y+FRAME_TICKER_HEIGHT+sourcePreviewHeight*3+ELEMENT_SPACER;
	maxwidthkeys->setDrawRect(ofRectangle(BUTTON_WIDTH, keyframeStartY, totalWidth, KEYFRAME_HEIGHT));
	//inbetween is the min/max visualizer
	minwidthkeys->setDrawRect(ofRectangle(BUTTON_WIDTH, keyframeStartY+KEYFRAME_HEIGHT+GRAPH_HEIGHT, totalWidth, KEYFRAME_HEIGHT));
	sourcekeys->setDrawRect(ofRectangle(BUTTON_WIDTH, keyframeStartY+KEYFRAME_HEIGHT*2+GRAPH_HEIGHT+ELEMENT_SPACER, totalWidth, SWITCHER_HEIGHT));
	blendkeys->setDrawRect(ofRectangle(BUTTON_WIDTH, keyframeStartY+KEYFRAME_HEIGHT*2+GRAPH_HEIGHT+SWITCHER_HEIGHT+ELEMENT_SPACER*2, totalWidth, KEYFRAME_HEIGHT));
	zoomer->setDrawRect(ofRectangle(BUTTON_WIDTH, keyframeStartY+KEYFRAME_HEIGHT*3+GRAPH_HEIGHT+SWITCHER_HEIGHT+ELEMENT_SPACER*3, totalWidth, FRAME_TICKER_HEIGHT));

//    cout << "CHECK 3: cur comp " << currentCompFolder << endl;


	//----------- BEGIN DRAWING
	ofPushStyle();

	//draw UI elements
	ofNoFill();
	ofSetColor(80, 80, 80);
	
	ofRect(newCompButton->x,newCompButton->y, newCompButton->width, newCompButton->height	);
	ofDrawBitmapString("new comp", newCompButton->x + 10, newCompButton->y + 15);

	ofRect(loadCompButton->x,loadCompButton->y, loadCompButton->width,loadCompButton->height);
	ofDrawBitmapString("load comp", loadCompButton->x + 10, loadCompButton->y + 15);

	float yStart = loadCompButton->y+loadCompButton->height+20;
	ofRect(loadCompButton->x,yStart, loadCompButton->width, 80);
	ofDrawBitmapString("Comp " + currentCompFolder + 
					   "\nSource A " + sourceA->directory +
					   "\nSource B " + sourceB->directory +
					   "\nDistortion " + distortion->directory +
					   "\nOutput " + exporter->pathPrefix, 
					   loadCompButton->x + 10, yStart + 15);

	
	ofRect(exportCurrentViewButton->x,exportCurrentViewButton->y, exportCurrentViewButton->width,exportCurrentViewButton->height);
	ofDrawBitmapString("export\ncurrent view", exportCurrentViewButton->x + 10, exportCurrentViewButton->y + 15);

	ofRect(exportEntireSequenceButton->x,exportEntireSequenceButton->y, exportEntireSequenceButton->width,exportEntireSequenceButton->height);
	ofDrawBitmapString("export\nentire sequence", exportEntireSequenceButton->x + 10, exportEntireSequenceButton->y + 15);

	ofRect(cancelExportButton->x,cancelExportButton->y, cancelExportButton->width,cancelExportButton->height);
	ofDrawBitmapString("cancel export", cancelExportButton->x + 10, cancelExportButton->y + 15);

    ofPopStyle();

	//draw frame ticker
    //TODO: put into timeline object
    int firstFrame = getFirstFrameInView();
    int lastFrame = getLastFrameInView();
    int totalFrames = (lastFrame - firstFrame);
	
	ofDrawBitmapString("First Frame:  " + ofToString(firstFrame) + "\n"+
					   "Last Frame:   " + ofToString(lastFrame) + "\n" +
					   "Total Frames: " + ofToString(totalFrames), frameInfoPosition);
	
	
    if(!allSequencesLoaded()){
        return;
    }

    ofPushStyle();
	//---------- PROGRESS


	ofNoFill();
	ofRect(uiposition.x, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*6, BIG_BUTTON_WIDTH, BIG_BUTTON_HEIGHT/2);
	if(exporting){
		float percentDone = exporter->getPercentDone();
		ofFill();
		ofSetColor(200, 200, 100, 40);
		ofRect(uiposition.x, uiposition.y+(BIG_BUTTON_HEIGHT+ELEMENT_SPACER)*6, BIG_BUTTON_WIDTH*percentDone, BIG_BUTTON_HEIGHT/2);
	}
	
	//draw frame numbers
	
	//---------- frame Info


    float pixelsPerFrame = (width-BUTTON_WIDTH*2.0) / totalFrames;
    int frameStep = 1;
    bool drawFullRect = false;
    if(pixelsPerFrame < .5){
        frameStep = 20;
        drawFullRect = true;
    }
    else if(pixelsPerFrame < 1.0){
        frameStep = 10;
        drawFullRect = true;
    }
    else if(pixelsPerFrame < 2.0){
        frameStep = 5;
    }
    else if(pixelsPerFrame < 4.0){
        frameStep = 2;
    }

    if(drawFullRect){
        ofSetColor(200, 200, 100, 40);
        ofRect(BUTTON_WIDTH, position.y+FRAME_TICKER_HEIGHT/2., totalWidth, FRAME_TICKER_HEIGHT/2);
    }

    ofSetColor(200, 200, 100, 100);
    for(int i = 0; i <= totalFrames; i+=frameStep){
        float xPixelPos = ofMap(i, 0, totalFrames, BUTTON_WIDTH, width-BUTTON_WIDTH, true);
        ofLine(xPixelPos, position.y+FRAME_TICKER_HEIGHT/2., xPixelPos, position.y+FRAME_TICKER_HEIGHT);
    }

	ofSetColor(255, 255, 255);
    sourceA->setDrawWidth(totalWidth);
	sourceB->setDrawWidth(totalWidth);
    distortion->setDrawWidth(totalWidth);

	//backgroup
	ofSetColor(0, 0, 0);
	ofRect(position.x + BUTTON_WIDTH, position.y+FRAME_TICKER_HEIGHT, totalWidth, sourcePreviewHeight*3);


	ofSetColor(255, 255, 255);
	sourceA->draw(position.x + BUTTON_WIDTH, position.y+FRAME_TICKER_HEIGHT);
	sourceB->draw(position.x + BUTTON_WIDTH, position.y+FRAME_TICKER_HEIGHT+sourcePreviewHeight);
	distortion->draw(position.x + BUTTON_WIDTH, position.y+FRAME_TICKER_HEIGHT+sourcePreviewHeight*2);


	ofEnableSmoothing();
    ofEnableAlphaBlending();
	ofSetColor(40, 40, 40);

	//---------- DRAW .5 LINE
	ofLine(BUTTON_WIDTH, sourcekeys->getDrawRect().y + .5*sourcekeys->getDrawRect().height,
		   BUTTON_WIDTH+totalWidth, sourcekeys->getDrawRect().y + .5*sourcekeys->getDrawRect().height);

	//--------DRAW MIN-MAX GRAPH VISUALIZATION
	float minMaxVisualizerOffset = keyframeStartY + KEYFRAME_HEIGHT;
	ofSetColor(200, 120, 0);
	for(int i = 0; i <= totalWidth; i+=4){
		//draw lines, then blank out with bars
		ofLine(BUTTON_WIDTH+i, minMaxVisualizerOffset, BUTTON_WIDTH+i, minMaxVisualizerOffset+GRAPH_HEIGHT);
	}


	ofSetColor(20, 20, 20);
	ofFill();
	for(int i = 0; i <= totalWidth; i+=2){
		float samplepos = ofMap(1.0f*i/totalWidth,0,1.0, zoomer->getViewRange().min, zoomer->getViewRange().max, true);
		//toprect
		ofRect(BUTTON_WIDTH+i, minMaxVisualizerOffset, 2, (1-maxwidthkeys->sampleTimelineAt(samplepos))*GRAPH_HEIGHT);
		//bottomrect
		float bottomheight = minwidthkeys->sampleTimelineAt(samplepos)*GRAPH_HEIGHT;
		ofRect(BUTTON_WIDTH+i, minMaxVisualizerOffset + (GRAPH_HEIGHT-bottomheight), 2, bottomheight);
	}


	//---------- DRAW KEYFRAMES
	maxwidthkeys->draw();
	minwidthkeys->draw();
	sourcekeys->draw();
	blendkeys->draw();
	zoomer->draw();

	if(loadedReferenceSource != NULL){
		ofSetColor(200, 200, 100, 100);
		//draw lines for preview points
		for(int i = 0; i < loadedReferenceSource->previewTextures.size(); i++){
			float xPixelPos = loadedReferenceSource->previewTextures[i].bounds.x + BUTTON_WIDTH;
			ofLine(xPixelPos, position.y, xPixelPos, position.y+FRAME_TICKER_HEIGHT+sourcePreviewHeight*3);
			ofDrawBitmapString(ofToString(loadedReferenceSource->previewTextures[i].frameIndex), xPixelPos+3, position.y+5);
		}
	}

    //--------------- DRAW PLAYHEAD
    if(rollingOverPlayhead){
	    ofSetColor(255, 0, 0, 100);
		float playheadX = position.x + BUTTON_WIDTH + totalWidth*playheadFloatPosition;
        ofLine(playheadX, position.y,
               playheadX, position.y+totalHeight-FRAME_TICKER_HEIGHT-ELEMENT_SPACER);

						   
		//draw distortion width
		if(loadedReferenceSource != NULL){
			ofSetColor(255, 0, 0, 50);
			float minXPercent = ofMap(getCurrentWindowStartFrame(), 0, getSequenceLength()-1, 0.0, 1.0, true);
			float maxXPercent = ofMap(getCurrentWindowEndFrame(), 0, getSequenceLength()-1, 0.0, 1.0, true);
			float minX = BUTTON_WIDTH + ofMap(minXPercent, zoomer->getViewRange().min, zoomer->getViewRange().max, 0, totalWidth, true);
			float maxX = BUTTON_WIDTH + ofMap(maxXPercent, zoomer->getViewRange().min, zoomer->getViewRange().max, 0, totalWidth, true);
			ofRect(minX, position.y+FRAME_TICKER_HEIGHT,  (maxX-minX), totalHeight-FRAME_TICKER_HEIGHT*2-ELEMENT_SPACER);
			
			ofSetColor(255, 200, 20);
			//draw frame ticker
			ofDrawBitmapString(ofToString(getCurrentWindowStartFrame()), ofPoint(minX, position.y-10)) ;

			//draw frame ticker
			ofDrawBitmapString(ofToString(getCurrentWindowEndFrame()), ofPoint(maxX, position.y+10)) ;
			
		}
		ofSetColor(255, 200, 20);
		//draw frame ticker
		ofDrawBitmapString(ofToString(loadedReferenceSource->getIndexAtPercent(zoomedPlayheadFloatPosition)), ofPoint(playheadX, position.y)) ;
		
    }

    ofPopStyle();
    return;

    ofSetColor(255, 255, 0, 255);
    ofLine(position.x + BUTTON_WIDTH + totalWidth*playheadPosition, position.y,
           position.x + BUTTON_WIDTH + totalWidth*playheadPosition, position.y+totalHeight-FRAME_TICKER_HEIGHT-ELEMENT_SPACER);

    ofNoFill();
    ofSetColor(255,0,255);
    ofRect(position.x + BUTTON_WIDTH, position.y+FRAME_TICKER_HEIGHT, totalWidth, sourcePreviewHeight*3);

    //------ DRAW DISABLED FIELD
	if(exporting){
		ofFill();
		ofSetColor(200, 200, 200, 20);
		ofRect(position.x+BUTTON_WIDTH, position.y, totalWidth, totalHeight);
	}

    ofPopStyle();

    sourceA->purgeThumbs();
    sourceB->purgeThumbs();
    distortion->purgeThumbs();
}

bool THISTimeline::allSequencesLoaded()
{
    return sourceA->isLoaded() && sourceB->isLoaded() && distortion->isLoaded();
}

int THISTimeline::getWindowStartFrame(int baseFrame)
{
	return getWindowStartFrame( (baseFrame+.5f) /(getSequenceLength()-1.0f) );
}

int THISTimeline::getWindowEndFrame(int baseFrame)
{
	return getWindowEndFrame( (baseFrame+.5f)/(getSequenceLength()-1.0f) );
}

int THISTimeline::getWindowStartFrame(float percent)
{
	if(!sourceA->isLoaded()){
		return 0;
	}

	return MAX(sourceA->getIndexAtPercent(percent) + ( getMinOffsetAtPercent(percent) - maxDistortionWidth/2.0 ), 0);
}

int THISTimeline::getWindowEndFrame(float percent)
{
	return MIN(getWindowStartFrame(percent) + getDistortionWidth( percent ), getSequenceLength()-1);
}

int THISTimeline::getCurrentWindowStartFrame()
{
	if(rollingOverPlayhead){
		return getWindowStartFrame(zoomedPlayheadFloatPosition);
	}
	return getWindowStartFrame(zoomedPlayheadPosition);
}

int THISTimeline::getCurrentWindowEndFrame()
{
	if(rollingOverPlayhead){
		return getWindowEndFrame(zoomedPlayheadFloatPosition);
	}
	return getWindowEndFrame(zoomedPlayheadPosition);
}

int THISTimeline::getMinOffsetAtPercent(float percent)
{
	return maxDistortionWidth * minwidthkeys->sampleTimelineAt(percent);
}

int THISTimeline::getFirstFrameInView()
{
	if(!sourceA->isLoaded()){
		return 0;
	}

	return sourceA->getIndexAtPercent(zoomer->getViewRange().min);
}

int THISTimeline::getLastFrameInView()
{
	if(!sourceA->isLoaded()){
		return 1;
	}
	return sourceA->getIndexAtPercent(zoomer->getViewRange().max);
}

int THISTimeline::getCurrentDistortionWidth()
{
	if(rollingOverPlayhead){
		return getDistortionWidth(zoomedPlayheadFloatPosition);
	}
	return getDistortionWidth(zoomedPlayheadPosition);
}

int THISTimeline::getDistortionWidth(int baseFrame)
{
	return getDistortionWidth(baseFrame/(getSequenceLength()-1.0f));
}

int THISTimeline::getDistortionWidth(float percent)
{
	return MAX(maxDistortionWidth*(maxwidthkeys->sampleTimelineAt(percent) - minwidthkeys->sampleTimelineAt(percent) ), 1);
}

unsigned char* THISTimeline::getSourcePixelsForFrame(int offset, bool thumb)
{
	float channel = sourcekeys->sampleTimelineAt(offset/(getSequenceLength()-1.0f));
	if(channel > .5){
		return sourceA->getPixelsForFrame(offset, thumb);
	}
	else{
		return sourceB->getPixelsForFrame(offset, thumb);
	}
}

float* THISTimeline::getCurrentBlendedDistortionFrame(bool thumb)
{
	if(rollingOverPlayhead){
		return getBlendedDistortionFrame(zoomedPlayheadFloatPosition, thumb);
	}
	return getBlendedDistortionFrame(zoomedPlayheadPosition, thumb);
}

float* THISTimeline::getBlendedDistortionFrame(float percent, bool thumb)
{
	int index = distortion->getIndexAtPercent(percent);
	if(index < 0 || index >= blendedDistortionThumbnails.size() || index >= blendedDistortion.size()){
		ofLog(OF_LOG_ERROR, "THISTimeline -- Asking for index %d when we only have %d frames", index, blendedDistortion.size());
		return NULL;
	}

	//cout << "asking for distortion at index " << index << " percent " << percent << endl;

	BlendedDistortionFrame* bdf = thumb ? &blendedDistortionThumbnails[index] : &blendedDistortion[index];
	int fullDistortionWidth = getDistortionWidth(percent);
	int taperedDistortionWidth = MAX(fullDistortionWidth * blendkeys->sampleTimelineAt(percent), 1);

	if(!bdf->loaded || bdf->distortionWidth != taperedDistortionWidth){

		int imgwidth  = thumb ? distortion->getThumbWidth()  : distortion->getImageWidth();
		int imgheight = thumb ? distortion->getThumbHeight() : distortion->getImageHeight();
		if(bdf->pixels == NULL){
			bdf->pixels = new float[imgwidth*imgheight];
		}
		memset(bdf->pixels, 0, sizeof(float)*imgwidth*imgheight);
		int midIndex = getWindowStartFrame(percent) + fullDistortionWidth/2;
		int startIndex = midIndex - (taperedDistortionWidth / 2);
		int endIndex = midIndex + ((1+taperedDistortionWidth) / 2);
		if(startIndex >= endIndex){
			endIndex = startIndex+1;
		}
		//add all the images together...
		for(int i = startIndex; i < MIN(endIndex, getSequenceLength()-1); i++){
			unsigned char* distortionPixels = distortion->getPixelsForFrame(i, thumb);
			for(int y = 0; y < imgheight; y++){
				for(int x = 0; x < imgwidth; x++){
					bdf->pixels[y*imgwidth+x] += distortionPixels[y*imgwidth+x];
				}
			}
		}

		//average
		for(int i = 0; i < imgwidth*imgheight; i++){
			bdf->pixels[i] /= (taperedDistortionWidth*255.0);
		}

		bdf->loaded = true;
		bdf->distortionWidth = taperedDistortionWidth;
	}

	bdf->lastUsedTime = ofGetElapsedTimef();
	return bdf->pixels;
}

float* THISTimeline::getBlendedDistortionFrame(int index, bool thumb)
{
	return getBlendedDistortionFrame(index/(getSequenceLength()-1.0f), thumb);
}

int THISTimeline::getSequenceLength()
{
	return MIN(MIN(sourceA->getTotalFrames(), sourceB->getTotalFrames()), distortion->getTotalFrames() );
}

ofTexture* THISTimeline::getCurrentPreviewFrame(THISSequence* source, ofTexture* previewFrame, int baseFrame)
{
    if(!source->isLoaded()){
		return NULL;
	}

	ofImage* frameImage = source->getFrame(baseFrame, true);

	if(frameImage == NULL){
		return NULL;
	}

	if(!previewFrame->bAllocated()){
		previewFrame->allocate(frameImage->getWidth(), frameImage->getHeight(), glTypeForImageType(frameImage->getPixelsRef().getImageType()));
	}
	previewFrame->loadData(frameImage->getPixels(), frameImage->getWidth(), frameImage->getHeight(), glTypeForImageType(frameImage->getPixelsRef().getImageType()));
    return previewFrame;
}

ofTexture* THISTimeline::getCurrentPreviewStartSourceAFrame()
{
	return getCurrentPreviewFrame(sourceA, previewStartSourceAFrame, getCurrentWindowStartFrame());
}

ofTexture* THISTimeline::getCurrentPreviewEndSourceAFrame()
{
	return getCurrentPreviewFrame(sourceA, previewEndSourceAFrame, getCurrentWindowEndFrame());
}

ofTexture* THISTimeline::getCurrentPreviewStartSourceBFrame()
{
	return getCurrentPreviewFrame(sourceB, previewStartSourceBFrame, getCurrentWindowStartFrame());
}

ofTexture* THISTimeline::getCurrentPreviewEndSourceBFrame()
{
	return getCurrentPreviewFrame(sourceB, previewEndSourceBFrame, getCurrentWindowEndFrame());
}

ofTexture* THISTimeline::getCurrentPreviewStartDistortionFrame()
{
	return getCurrentPreviewFrame(distortion, previewStartDistortionFrame, getCurrentWindowStartFrame());
}

ofTexture* THISTimeline::getCurrentPreviewEndDistortionFrame()
{
	return getCurrentPreviewFrame(distortion, previewEndDistortionFrame, getCurrentWindowEndFrame());
}

ofTexture* THISTimeline::getCurrentPreviewBlendedDistortionFrame()
{
    if(!distortion->isLoaded()){
		return NULL;
	}

	if(distortionPreviewPixelsChar == NULL){
		distortionPreviewPixelsChar = new unsigned char[ int(distortion->getThumbWidth()*distortion->getThumbHeight()) ];
    }

	float percent = rollingOverPlayhead ? zoomedPlayheadFloatPosition : zoomedPlayheadPosition;
	float* pix = getBlendedDistortionFrame(percent, true);
	for(int i = 0; i < distortion->getThumbWidth()*distortion->getThumbHeight(); i++){
		distortionPreviewPixelsChar[i] = int(pix[i]*255.0);
	}

	if(!previewBlendedDistortionFrame->bAllocated()){
		previewBlendedDistortionFrame->allocate(distortion->getThumbWidth(), distortion->getThumbHeight(), GL_LUMINANCE);
	}

	previewBlendedDistortionFrame->loadData(distortionPreviewPixelsChar, distortion->getThumbWidth(), distortion->getThumbHeight(), GL_LUMINANCE);
	return previewBlendedDistortionFrame;
}

ofTexture* THISTimeline::getCurrentPreviewOutputFrame()
{
    if(!distortion->isLoaded() || !sourceA->isLoaded() || !sourceB->isLoaded()){
        return NULL;
    }

	ofImage* outputframe = output->renderOutputFrame(true);
	if(outputframe == NULL){
		return NULL;
	}

	if(!previewOutputFrame->bAllocated()){
		previewOutputFrame->allocate(outputframe->getWidth(), outputframe->getHeight(), glTypeForImageType(outputframe->getPixelsRef().getImageType()));
	}
	previewOutputFrame->loadData(outputframe->getPixels(), outputframe->getWidth(), outputframe->getHeight(), glTypeForImageType(outputframe->getPixelsRef().getImageType()));
	return previewOutputFrame;
}


void THISTimeline::purgeMemoryForExport()
{
	distortion->purgeFrames();
	sourceA->purgeFrames();
	sourceB->purgeFrames();
	vector<BlendedDistortionFrame*> blendedDistortionSorted;

	for(int i = 0; i < blendedDistortion.size(); i++){
		if(blendedDistortion[i].loaded){
			blendedDistortionSorted.push_back( &blendedDistortion[i] );
		}
	}

	if(blendedDistortionSorted.size() > MAX_BLENDED_DISTORTION){
		sort(blendedDistortionSorted.begin(), blendedDistortionSorted.end(), blendedframesort);

		int numToDelete = blendedDistortionSorted.size() - MAX_BLENDED_DISTORTION;

		//cout << "Deleteing " << numToDelete << " blended with age " << (ofGetElapsedTimef() - blendedDistortionSorted[0]->lastUsedTime) << endl;

		for(int i = 0; i < numToDelete; i++){
			blendedDistortionSorted[i]->loaded = false;
			delete blendedDistortionSorted[i]->pixels;
			blendedDistortionSorted[i]->pixels = NULL;
		}
	}
}

void THISTimeline::objectDidRollOver(ofxMSAInteractiveObject* object, int x, int y)
{
	if(exporting) return;

    if(object == playheadBar){
	    rollingOverPlayhead	= true;
		isPlayingBack = false;
    }
}

void THISTimeline::objectDidRollOut(ofxMSAInteractiveObject* object, int x, int y)
{
	if(exporting) return;

    if(object == playheadBar){
	    rollingOverPlayhead	= false;
		isPlayingBack = false;
    }
}

void THISTimeline::objectDidPress(ofxMSAInteractiveObject* object, int x, int y, int button)
{
	if(exporting) return;

    if(object == playheadBar){
        playheadPosition = ofMap(x, object->x,object->x+object->width, 0, 1.0, true);
		zoomedPlayheadPosition = ofMap(x, object->x,object->x+object->width, zoomer->getViewRange().min, zoomer->getViewRange().max, true);

        settings.setValue("settings:playhead", playheadPosition);
        settings.saveFile(settingsFileName);
    }

	//cout << "object did press " << endl;
}

void THISTimeline::objectDidMouseMove(ofxMSAInteractiveObject* object, int x, int y)
{
	if(exporting) return;

    if(object == playheadBar){
        playheadFloatPosition = ofMap(x, object->x,object->x+object->width, 0, 1.0, true);
		zoomedPlayheadFloatPosition = ofMap(x, object->x,object->x+object->width, zoomer->getViewRange().min, zoomer->getViewRange().max, true);

    	//cout << "playhead hover " << x << " " << p << endl;
    }
}

void THISTimeline::objectDidRelease(ofxMSAInteractiveObject* object, int x, int y, int button)
{
	if(object == cancelExportButton){
		exporter->cancelExport();
		return;
	}
	
	if(exporting){
		return;
	}
	
	if(object == newCompButton){
		newComposition();
	}
	else if(object == loadCompButton){
		ofFileDialogResult result = ofSystemLoadDialog("Load Composition", true);
		if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
			loadComposition(result.getPath());
		}
	}

	if(object == exportEntireSequenceButton){
		exportEntireSequence();
	}

	if(object == exportCurrentViewButton){
		exportCurrentView();
	}
}

void THISTimeline::newComposition(){
	ofSystemAlertDialog("Choose where to save your comp files");
	string newWorkingDirectory = "";
	string newADir = "";
	string newBDir = "";
	string newDistortDir = "";
	string newOutDir = "";
	ofFileDialogResult result;
	result = ofSystemLoadDialog("Choose Comp Directory", true);
	if(result.bSuccess){
		newWorkingDirectory = result.filePath;
	}
	else{
		ofSystemAlertDialog("You didn't select a Comp directory.");
		return;
	}
	
	ofSystemAlertDialog("Choose SOURCE A sequence folder");
	result = ofSystemLoadDialog("Choose SOURCE A sequence folder", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		newADir = result.filePath;
	}
	else{
		ofSystemAlertDialog("Failed to load Source A Sequence directory");
		return;
	}
	
	ofSystemAlertDialog("Choose SOURCE B sequence folder");
	result = ofSystemLoadDialog("Choose SOURCE B sequence folder", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		newBDir = result.filePath;
	}
	else{
		ofSystemAlertDialog("Failed to load Source B Sequence");
		return;
	}
	
	ofSystemAlertDialog("Choose DISTORTION sequence folder");
	result = ofSystemLoadDialog("Choose DISTORTION sequence folder", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		newDistortDir = result.filePath;
	}
	else{
		ofSystemAlertDialog("Failed to load Distortion Sequence");
		return;
	}
	
	ofSystemAlertDialog("Set OUTPUT Directory");			
	result = ofSystemLoadDialog("Set Output Directory", true);
	if(result.bSuccess && ofDirectory::doesDirectoryExist(result.filePath, false)){
		newOutDir = result.filePath;
	}
	else{
		ofSystemAlertDialog("Failed to select output directory");
		return;
	}
	
	setWorkingFolder(newWorkingDirectory);
	settings.setValue("settings:source_directory_a", newADir);		
	settings.setValue("settings:source_directory_b", newBDir);
	settings.setValue("settings:distortion_directory", newDistortDir);
	exporter->pathPrefix = newOutDir;
	settings.setValue("settings:output_directory", newOutDir);
	
	loadComposition(settings);	
	settings.saveFile(settingsFileName);

}

void THISTimeline::createDistortionCaches()
{
	for(int i = 0; i < distortion->getTotalFrames(); i++){
		BlendedDistortionFrame frame;
		frame.pixels = NULL;
		frame.distortionWidth = maxDistortionWidth;
		frame.lastUsedTime = ofGetElapsedTimef();
		frame.loaded = false;
		frame.index = i;

		blendedDistortion.push_back( frame );
		blendedDistortionThumbnails.push_back( frame );
	}
}

void THISTimeline::clearDistortionCaches()
{
	for(int i = 0; i < blendedDistortion.size(); i++){
		if(blendedDistortion[i].loaded){
			delete blendedDistortion[i].pixels;
		}
		if(blendedDistortionThumbnails[i].loaded){
			delete blendedDistortionThumbnails[i].pixels;
		}
	}
	blendedDistortion.clear();
	blendedDistortionThumbnails.clear();
}

void THISTimeline::togglePlayback()
{
	if(exporting) return;

	if(isPlayingBack){
		isPlayingBack = false;
	}
	else{
		isPlayingBack = true;
		rollingOverPlayhead = true;
		playbackStartTime = ofGetElapsedTimef();
		percentPerFrame = 1. / getSequenceLength();
	}
}


void THISTimeline::exportEntireSequence()
{
	exporter->startFrame = 0;
	exporter->endFrame = getSequenceLength()-1;

	startExport();
}

void THISTimeline::exportCurrentView()
{
	exporter->startFrame = getFirstFrameInView();
	exporter->endFrame = getLastFrameInView();

	startExport();
}

void THISTimeline::startExport()
{
	minwidthkeys->disable();
	maxwidthkeys->disable();
	sourcekeys->disable();
	blendkeys->disable();
	zoomer->disable();

	//TODO: mulithread
	exporting = true;
	isPlayingBack = false;
	exporter->startThread(true, false);

}
