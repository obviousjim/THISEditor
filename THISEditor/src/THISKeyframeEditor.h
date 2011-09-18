/*
 *  THISKeyframeEditor.h
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxTween.h"
#include "ofRange.h"

typedef struct
{
	int id;
	ofRectangle bounds;
	string name;
	ofxEasing* easing;
} EasingFunction;

typedef struct
{
	int id;
	ofRectangle bounds;
	string name;
	ofxTween::ofxEasingType type;
} EasingType;

typedef struct
{
	EasingFunction* easeFunc;
	EasingType* easeType;
	ofVec2f position; // x is value, y is time, all 0 - 1.0
} THISKeyframe;

class THISKeyframeEditor
{
  public:	
	THISKeyframeEditor();
	~THISKeyframeEditor();

	void setDrawRect(ofRectangle bounds);
	void setZoomBounds(ofRange zoomBoundsPercent); //allows you to zoom in!
	ofRectangle getDrawRect();
	
	//main function to get values out of the timeline
	float sampleTimelineAt(float percent);
	
	void draw();
	
	void enable();
	void disable();
	
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
		
	void keyPressed(ofKeyEventArgs& args);
	
	void setXMLFileName(string filename);
	void loadFromXML();
	void saveToXML();
	void setAutosave(bool autosave);
	
	void reset();
	void clear();
	
	int durationInFrames;
	bool snapToFrame;

  private:		
	string xmlFileName;
	
	bool autosave;
	bool hasFocus;
	bool enabled;
	
	THISKeyframe* firstkey;
	THISKeyframe* lastkey;
	ofVec2f grabOffset;
	vector<THISKeyframe*> keyframes;
	ofRectangle bounds;
	ofRange zoomBounds;
	
	bool keyframeIsInBounds(THISKeyframe* key);
	
	THISKeyframe* selectedKeyframe;
	int selectedKeyframeIndex;
	ofVec2f keyframeGrabOffset;
	float minBound;
	float maxBound;
	
	void updateKeyframeSort();
	THISKeyframe* keyframeAtScreenpoint(ofVec2f p, int& selectedIndex);
	bool screenpointIsInBounds(ofVec2f screenpoint);
	ofVec2f coordForKeyframePoint(ofVec2f keyframePoint);
	ofVec2f keyframePointForCoord(ofVec2f coord);
		
	//easing dialog stuff
	ofVec2f easingWindowPosition;
	bool drawingEasingWindow;
	vector<EasingFunction*> easingFunctions;
	vector<EasingType*> easingTypes;
	float easingBoxWidth;
	float easingBoxHeight;
	float easingWindowSeperatorHeight;
	
	void initializeEasings();
	THISKeyframe* newKeyframe(ofVec2f point);
};