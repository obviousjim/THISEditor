/*
 *  THISFrame.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#include "THISFrame.h"
#include "ofFileUtils.h"

THISFrame::THISFrame()
{

	filename = "";
	
	frame = new ofImage();
	frame->setUseTexture(false);
	thumbnail = new ofImage();
	thumbnail->setUseTexture(false);
	
	frameLoaded = false;
	thumbLoaded = false;
	lastUsedTime = ofGetElapsedTimef();
	desiredThumbWidth = 1280/4;
	
	type = -1;
}

THISFrame::~THISFrame()
{
	frame->clear();
	thumbnail->clear();
	delete frame;
	delete thumbnail;
}

void THISFrame::setFrame(string _filename)
{
	filename = _filename;
    //to write to
    string path = ofFileUtils::getEnclosingDirectoryFromPath( _filename );
    shortFilename = ofFileUtils::getFilenameFromPath( _filename );
    
    thumbFilename = path + "thumbs/" + shortFilename;
}

ofImage* THISFrame::getFrame()
{
	if(!frameLoaded){
		loadFrame();
	}
	
	lastUsedTime = ofGetElapsedTimef();

//	cout << "last used time " << shortFilename << " is " << lastUsedTime << endl;
	
	return frame;
}

ofImage* THISFrame::getThumbnail()
{
	if(!thumbLoaded){
		loadThumb();
	}	
	
	//useCount = 0;
	lastUsedTime = ofGetElapsedTimef();
	//usedThisFrame = true;

	return thumbnail;
}

bool THISFrame::isFrameLoaded()
{
	return frameLoaded;
}

bool THISFrame::isThumbLoaded()
{
	return thumbLoaded;
}

void THISFrame::clear()
{
	frame->clear();
	thumbnail->clear();
    frame->setUseTexture(false);
	thumbnail->setUseTexture(false);
	
	frameLoaded = false;
	thumbLoaded = false;
	lastUsedTime = ofGetElapsedTimef();
}

void THISFrame::setType(int _type)
{
	type = _type;
}

bool THISFrame::loadFrame()
{
	if(filename == ""){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - Loading frame without file name");	
		return false;	
	}

	if(!frame->loadImage(filename)){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - failed to load file " + filename);
		return false;
	}
	
	if(type != -1){
		frame->setImageType(type);
	}
	
	frameWidth = frame->getWidth();
	frameHeight = frame->getHeight(); 
	
	frameLoaded = true;
	return true;
}

void THISFrame::setDesiredThumbnailWidth(int width)
{
	desiredThumbWidth = width;
}

bool THISFrame::loadThumb()
{
	if(filename == ""){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - Loading frame without file name " + filename);	
		return false;
	}
	
    //first check if there is a thumbnail
	if(!ofFileUtils::doesFileExist(thumbFilename)){
		return generateThumbnail();
    }
    else{
		if(thumbnail->loadImage(thumbFilename)){
			if(thumbnail->getWidth() != desiredThumbWidth){
				return generateThumbnail();
			}
			else {
				thumbWidth = thumbnail->getWidth();
				thumbHeight = thumbnail->getHeight(); 
			}
		}
		else{
			ofLog(OF_LOG_ERROR, "THISFrame - ERROR - thumbnail " + thumbFilename + " failed to load!" );
			return false;
		}
    }
	
    thumbLoaded = true;
	return true;
}

bool THISFrame::generateThumbnail()
{
	//load and create thumbnail
	bool frameAlreadyLoaded = frameLoaded;
	bool success = frameAlreadyLoaded || loadFrame();
	if(!success){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - failed to load file " + filename);
		return false;
	}
	thumbnail->clone(*frame);
	thumbWidth = desiredThumbWidth;
	float scaleFactor = 1.0*frame->getWidth() / thumbWidth;
	thumbHeight = frame->getHeight() / scaleFactor;
	thumbnail->resize(thumbWidth, thumbHeight);	
	thumbnail->saveImage(thumbFilename);		
	if(!frameAlreadyLoaded){
		frame->clear();
		frame->setUseTexture(false);
		frameLoaded = false;
	}
	thumbLoaded = true;
	return true;
}

int THISFrame::getFullFrameWidth()
{
	return frameWidth;
}

int THISFrame::getFullFrameHeight()
{
    return frameHeight;
}

int THISFrame::getThumbWidth()
{
    return thumbWidth;
}

int THISFrame::getThumbHeight()
{
    return thumbHeight;
}
