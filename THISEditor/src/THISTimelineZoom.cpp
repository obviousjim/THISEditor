/*
 *  THISTimelineZoom.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 6/26/11.
 *  Copyright 2011 FlightPhase. All rights reserved.
 *
 */

#include "THISTimelineZoom.h"
#include "ofxXmlSettings.h"
#include "ofRange.h"

THISTimelineZoom::THISTimelineZoom()
{
	hasFocus = false;
	autosave = true;
	xmlFileName = "_zoomsettings.xml";

	minSelected = false;
	maxSelected = false;
	midSelected = false;

	currentViewRange = ofRange(0.0, 1.0);

	ofAddListener(ofEvents.mouseMoved, this, &THISTimelineZoom::mouseMoved);
	ofAddListener(ofEvents.mousePressed, this, &THISTimelineZoom::mousePressed);
	ofAddListener(ofEvents.mouseReleased, this, &THISTimelineZoom::mouseReleased);
	ofAddListener(ofEvents.mouseDragged, this, &THISTimelineZoom::mouseDragged);

	ofAddListener(ofEvents.keyPressed, this, &THISTimelineZoom::keyPressed);

	enabled = true;
}

void THISTimelineZoom::setDrawRect(ofRectangle drawRect)
{
	bounds = drawRect;
}

void THISTimelineZoom::draw()
{
	ofPushStyle();
	ofEnableSmoothing();

	ofNoFill();
	if(hasFocus){
		ofSetColor(255, 200, 0); //focused outline color
	}
	else{
		ofSetColor(150, 150, 0); //unfocused outline color
	}

	ofRect(bounds.x, bounds.y, bounds.width, bounds.height);

	//draw min
	float screenY = bounds.y + bounds.height/2.0;
	float minScreenX = rangeXtoScreenX(currentViewRange.min);
	float maxScreenX = rangeXtoScreenX(currentViewRange.max);

	if(midSelected){
		ofSetLineWidth(2);
	}
	else{
		ofSetLineWidth(1);
	}

	ofLine(minScreenX, screenY, maxScreenX, screenY);
	ofSetLineWidth(1);

	if(minSelected){
		ofFill();
	}
	else{
		ofNoFill();
	}
	ofCircle(minScreenX, screenY, 5);

	if(maxSelected){
		ofFill();
	}
	else{
		ofNoFill();
	}
	ofCircle(maxScreenX, screenY, 5);

	ofPopStyle();
}

void THISTimelineZoom::setXmlFileName(string filename)
{
	xmlFileName = filename;
}

void THISTimelineZoom::loadFromXML()
{
	ofxXmlSettings settings;
	if(!settings.loadFile(xmlFileName)){
		ofLog(OF_LOG_ERROR, "THISTimelineZoom -- couldn't load zoom settings file");
		return;
	}

	settings.pushTag("zoom");
	currentViewRange = ofRange(settings.getValue("min", 0.0),
							   settings.getValue("max", 1.0));

	settings.popTag();
}

void THISTimelineZoom::saveToXML()
{
	ofxXmlSettings savedSettings;
	savedSettings.addTag("zoom");
	savedSettings.pushTag("zoom");
	savedSettings.addValue("min", currentViewRange.min);
	savedSettings.addValue("max", currentViewRange.max);
	savedSettings.popTag();//zoom
	savedSettings.saveFile(xmlFileName);
}

void THISTimelineZoom::setAutosave(bool doauto)
{
	autosave = doauto;
}

void THISTimelineZoom::enable()
{
	enabled = true;
}

void THISTimelineZoom::disable()
{
	enabled = false;
	hasFocus = false;
}

void THISTimelineZoom::mousePressed(ofMouseEventArgs& args)
{
	if(!enabled) return;

	minSelected = maxSelected = midSelected = hasFocus  = false;
	if (pointInScreenBounds(ofVec2f(args.x, args.y))) {
		mouseIsDown = true;
		hasFocus = true;
		float minScreenX = rangeXtoScreenX(currentViewRange.min);
		minGrabOffset = args.x - minScreenX ;
		if(fabs(minScreenX - args.x) < 5){
			minSelected = true;
			return;
		}

		float maxScreenX = rangeXtoScreenX(currentViewRange.max);
		maxGrabOffset = args.x - maxScreenX;
		if(fabs(maxScreenX - args.x) < 5){
			maxSelected = true;
			return;
		}

		if(args.x > minScreenX && args.x < maxScreenX){
			midSelected = true;
		}
	}
}

void THISTimelineZoom::mouseMoved(ofMouseEventArgs& args)
{

}

void THISTimelineZoom::mouseDragged(ofMouseEventArgs& args)
{
	if(!enabled) return;

	if(minSelected || midSelected){
		currentViewRange.min = ofClamp( screenXtoRangeX(args.x-minGrabOffset), 0, currentViewRange.max);
	}

	if(maxSelected || midSelected){
		currentViewRange.max = ofClamp( screenXtoRangeX(args.x-maxGrabOffset), currentViewRange.min, 1.0);
	}
}

bool THISTimelineZoom::isActive()
{
	return mouseIsDown && ( maxSelected || minSelected || midSelected);
}

void THISTimelineZoom::mouseReleased(ofMouseEventArgs& args)
{
	if(!enabled) return;

	mouseIsDown = false;
	if(autosave){
		saveToXML();
	}
}

void THISTimelineZoom::keyPressed(ofKeyEventArgs& args)
{
}

ofRange THISTimelineZoom::getViewRange()
{
	return currentViewRange;
}

bool THISTimelineZoom::pointInScreenBounds(ofVec2f screenpoint)
{
	return screenpoint.x > bounds.x && screenpoint.y > bounds.y && screenpoint.x < bounds.x+bounds.width && screenpoint.y < bounds.y+bounds.height;
}

float THISTimelineZoom::screenXtoRangeX(float x)
{
	return ofMap(x, bounds.x, bounds.x+bounds.width, 0.0, 1.0, true);
}

float THISTimelineZoom::rangeXtoScreenX(float x)
{
	return ofMap(x, 0.0, 1.0, bounds.x, bounds.x+bounds.width, true);
}
