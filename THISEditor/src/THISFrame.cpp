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

	type = OF_IMAGE_UNDEFINED;
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
    string path = ofFilePath::getEnclosingDirectory( _filename );
    shortFilename = ofFilePath::getFileName( _filename );

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

void THISFrame::setType(ofImageType _type)
{
	type = _type;
}

bool THISFrame::loadFrame()
{
    if(frameLoaded){
        return true;
    }
	if(filename == ""){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - Loading frame without file name");
		return false;
	}

	if(!frame->loadImage(filename)){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - failed to load file " + filename);
		return false;
	}

    //do this before converting the image type
    if(!thumbLoaded){
        thumbnail->clear();
        thumbnail->setUseTexture(false);
        thumbnail->clone(*frame);
        thumbWidth = desiredThumbWidth;
        float scaleFactor = 1.0*frame->getWidth() / thumbWidth;
        thumbHeight = frame->getHeight() / scaleFactor;
        thumbnail->resize(thumbWidth, thumbHeight);
        thumbnail->saveImage(thumbFilename);
        if(type != OF_IMAGE_UNDEFINED && thumbnail->getPixelsRef().getImageType() != type){
			cout << "WARNING CHANGING THUMB TYPE FROM " << thumbnail->getPixelsRef().getImageType() << " to " << type << endl;
            thumbnail->setImageType(type);
        }
        thumbLoaded = true;
    }
	
		
	if(type != OF_IMAGE_UNDEFINED && frame->getPixelsRef().getImageType() != type){
		cout << "WARNING CHANGING FRAME TYPE FROM " << thumbnail->getPixelsRef().getImageType() << " to " << type << endl;
		frame->setImageType(type);
	}

	cout << "file name is " << filename << " frame type is " << frame->getPixelsRef().getImageType() << endl;

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
    if(thumbLoaded){
        return true;
    }

	if(filename == ""){
		ofLog(OF_LOG_ERROR, "THISFrame - ERROR - Loading frame without file name");
		return false;
	}

    //first check if there is a thumbnail
	if(!ofFile::doesFileExist(thumbFilename)){
		return loadFrame();
    }
    thumbnail->clear();
    thumbnail->setUseTexture(false);
    if(!thumbnail->loadImage(thumbFilename)){
        ofLog(OF_LOG_ERROR, "THISFrame - ERROR - thumbnail file exists " + thumbFilename + " but was corrputed. reloading." );
        return loadFrame();
    }

    if(thumbnail->getWidth() != desiredThumbWidth){
        ofLog(OF_LOG_ERROR, "THISFrame - ERROR - thumbnail file exists " + thumbFilename + " but is the wrong size. reloading." );
        return loadFrame();
    }

	if(type != OF_IMAGE_UNDEFINED && thumbnail->getPixelsRef().getImageType() != type){
		cout << "WARNING CHANGING THUMB TYPE" << endl;
		thumbnail->setImageType( type );
    }
	
    thumbWidth = thumbnail->getWidth();
    thumbHeight = thumbnail->getHeight();

    thumbLoaded = true;
	return true;
}


/*
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
	thumbnail->saveImage(thumbFilename, OF_IMAGE_QUALITY_MEDIUM);
	if(!frameAlreadyLoaded){
		frame->clear();
		frame->setUseTexture(false);
		frameLoaded = false;
	}
	thumbLoaded = true;
	return true;
}
*/

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
