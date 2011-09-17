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
#include "ofxVectorMath.h"
#include "ofxTween.h"

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
	ofxVec2f position; // x is value, y is time, all 0 - 1.0
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
	
  private:		
	string xmlFileName;
	
	bool autosave;
	bool hasFocus;
	bool enabled;
	
	THISKeyframe* firstkey;
	THISKeyframe* lastkey;
	ofxVec2f grabOffset;
	vector<THISKeyframe*> keyframes;
	ofRectangle bounds;
	ofRange zoomBounds;
	
	bool keyframeIsInBounds(THISKeyframe* key);
	
	THISKeyframe* selectedKeyframe;
	int selectedKeyframeIndex;
	ofxVec2f keyframeGrabOffset;
	float minBound;
	float maxBound;
	
	void updateKeyframeSort();
	THISKeyframe* keyframeAtScreenpoint(ofxVec2f p, int& selectedIndex);
	bool screenpointIsInBounds(ofxVec2f screenpoint);
	ofxVec2f coordForKeyframePoint(ofxVec2f keyframePoint);
	ofxVec2f keyframePointForCoord(ofxVec2f coord);
		
	//easing dialog stuff
	ofxVec2f easingWindowPosition;
	bool drawingEasingWindow;
	vector<EasingFunction*> easingFunctions;
	vector<EasingType*> easingTypes;
	float easingBoxWidth;
	float easingBoxHeight;
	float easingWindowSeperatorHeight;
	
	void initializeEasings();
	THISKeyframe* newKeyframe(ofxVec2f point);
};