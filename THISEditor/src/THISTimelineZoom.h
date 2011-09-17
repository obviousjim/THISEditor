/*
 *  THISTimelineZoom.h
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofRange.h"

class THISTimelineZoom
{
  public:
	THISTimelineZoom();
	
	void setDrawRect(ofRectangle draw);
	void draw();
	
	void setXmlFileName(string filename);
	void loadFromXML();
	void saveToXML();
	void setAutosave(bool autosave);
	
	void enable();
	void disable();
	
	ofRange getViewRange();

	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	
	void keyPressed(ofKeyEventArgs& args);
	
	bool isActive();
	
  private:
	bool hasFocus;
	bool autosave;
	bool enabled;
	
	string xmlFileName;
	ofRange currentViewRange;
	ofRectangle bounds;
	
	float minGrabOffset;
	float maxGrabOffset;
	
	bool mouseIsDown;
	bool minSelected;
	bool maxSelected;
	bool midSelected;
	
	bool pointInScreenBounds(ofVec2f screenpoint);
	float screenXtoRangeX(float x);
	float rangeXtoScreenX(float x);
};