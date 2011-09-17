/*
 *  THISSequence.cpp
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#include "THISSequence.h"
#include "ofFileUtils.h"

#define MAX_FRAMES 75
#define MAX_THUMBS 500

bool framesort(THISFrame* a, THISFrame* b)
{
    return a->lastUsedTime < b->lastUsedTime;
}

THISSequence::THISSequence()
{
	drawRange = ofRange(0.0, 1.0);
	loaded = false;
	widthSet = false;
	imageType = OF_IMAGE_COLOR;
	drawWidth = -1;
	//TODO: set through interface
	widthPerFrame = 75;

}

THISSequence::~THISSequence()
{
	clear();
}

void THISSequence::loadSequence(string directoryName)
{
    if(loaded){
		clear();
    }
    cout << "LOADING SEQUENCE " << directoryName << endl;

    loaded = false;

	ofDirectory list;
	list.allowExt("png");
	list.allowExt("jpg");

    directory = directoryName;
	int numFiles = list.listDir(directory);
	if(numFiles == 0){
		ofLog(OF_LOG_ERROR, "THIS_Sequence -- ERROR -- Loaded sequence with no valid frames " + directory);
		return;
	}

	cout << "FOUND " << numFiles << endl;

    //create thumb directory
    if(!ofDirectory::doesDirectoryExist(directory+"/thumbs/")){
        ofDirectory::createDirectory(directory+"/thumbs/");
    }

	for(int i = 0; i < numFiles; i++){
		string frameFileName =  list.getPath(i);
		THISFrame* frame = new THISFrame();
		frame->setType(imageType);
		frame->setFrame(frameFileName);
		frames.push_back(frame);
	}

	cout << "CREATING FRAMES " << numFiles << endl;

    frames[0]->loadFrame();

	imageWidth = frames[0]->getFullFrameWidth();
    imageHeight = frames[0]->getFullFrameHeight();

	thumbWidth = frames[0]->getThumbWidth();
    thumbHeight = frames[0]->getThumbHeight();

	loaded = true;

    recomputePreview();

    cout << "DONE CREATING FRAMES " << numFiles << " img " << imageWidth << "x" << imageHeight << " thumb " << thumbWidth << "x" << thumbHeight << endl;

}

void THISSequence::clear()
{
	clearPreviewTextures();
	clearFrames();
	loaded = false;
}

void THISSequence::clearPreviewTextures()
{
	for(int i = 0; i < previewTextures.size(); i++){
		delete previewTextures[i].texture;
	}
	previewTextures.clear();
}

void THISSequence::clearFrames()
{
	for(int i = 0; i < frames.size(); i++){
		delete frames[i];
	}
	frames.clear();
}

void THISSequence::loadSequence()
{
	ofFileDialogResult result = ofSystemLoadDialog("Load Sequence", true);
	if(result.bSuccess){
		directory = result.getPath();
		loadSequence(directory);
	}
	else{
		ofLog(OF_LOG_ERROR, "THIS_Sequence -- ERROR -- attempted loaded invalid directory '" + directory + "'.");
	}
}

void THISSequence::setImageType(ofImageType type)
{
	imageType = type;
}

void THISSequence::setDrawRange(ofRange newRange)
{
	if(newRange != drawRange){
		drawRange = newRange;
		recomputePreview();
	}
}


void THISSequence::setDrawWidth(float _width)
{
	if(_width != drawWidth){
		drawWidth = _width;
		widthSet = true;
		recomputePreview();
	}
}

void THISSequence::recomputePreview()
{
	if(!loaded || !widthSet){
		return;
	}

	framesToShow = (drawWidth / widthPerFrame);

	int startIndex = getIndexAtPercent(drawRange.min);
	int endIndex = getIndexAtPercent(drawRange.max);
	int framesInRange = (endIndex - startIndex);

	heightPerFrame = (widthPerFrame/imageWidth) * imageHeight;

	int frameStep = framesInRange / framesToShow;
	int fameIndex = 0;

	clearPreviewTextures();

	for(int i = 0; i < framesToShow; i ++){
		ofImage* thumbnail = frames[startIndex+frameStep*i]->getThumbnail();
		PreviewTexture p;
		p.frameIndex = startIndex+frameStep*i;
		p.texture = new ofTexture();
		p.texture->allocate(thumbnail->getWidth(), thumbnail->getHeight(), glTypeForImageType(imageType));
		p.texture->loadData(thumbnail->getPixels(), thumbnail->getWidth(), thumbnail->getHeight(), glTypeForImageType(imageType));
		p.bounds = ofRectangle(widthPerFrame*i, 0, widthPerFrame, heightPerFrame);


		previewTextures.push_back( p );
	}
}

void THISSequence::draw(int x, int y)
{

	if(!loaded || !widthSet){
		return;
	}

    //cout << "preview textures size is " << previewTextures.size() << " " << endl;

	for(int i = 0; i < previewTextures.size(); i++){
		ofRectangle b = previewTextures[i].bounds;
		previewTextures[i].texture->draw(x + b.x, y, b.width, b.height);
	}
}

void THISSequence::purgeFrames()
{
	vector<THISFrame*> sortableFrames;
	for(int i = 0; i < frames.size(); i++){
        if(frames[i]->isFrameLoaded()){
			sortableFrames.push_back( frames[i] );
		}
	}

	if(sortableFrames.size() == 0){
		return;
	}

	sort(sortableFrames.begin(), sortableFrames.end(), framesort);

	cout << " currently " << sortableFrames.size() << " frames loaded. Oldest: " << (ofGetElapsedTimef() - sortableFrames[0]->lastUsedTime) << ". Newest: " << (ofGetElapsedTimef() - sortableFrames[sortableFrames.size()-1]->lastUsedTime) << endl;

    if(sortableFrames.size() > MAX_FRAMES){

		//cout << "total loaded " << totalLoaded << " use count is " << maxUseCount << " total this frame " << usedThisFrame << " " << thisframefilename <<  endl;
        int numToClear = sortableFrames.size() - MAX_FRAMES;

		for(int i = 0; i < numToClear; i++){
			sortableFrames[i]->clear();
		}
    }
}

void THISSequence::purgeThumbs()
{
	vector<THISFrame*> sortableFrames;
	for(int i = 0; i < frames.size(); i++){
        if(frames[i]->isThumbLoaded()){
			sortableFrames.push_back( frames[i] );
		}
	}

    if(sortableFrames.size() > MAX_THUMBS){

		//cout << "total loaded " << totalLoaded << " use count is " << maxUseCount << " total this frame " << usedThisFrame << " " << thisframefilename <<  endl;

		sort(sortableFrames.begin(), sortableFrames.end(), framesort);

        int numToClear = sortableFrames.size() - MAX_THUMBS;
		cout << "purging " << numToClear << " thumbs.  Oldest: " << sortableFrames[0]->lastUsedTime << endl;
		for(int i = 0; i < numToClear; i++){
			sortableFrames[i]->clear();
		}
    }
}

int THISSequence::getIndexAtPercent(float percent)
{
	return ofMap(percent, 0, 1.0, 0, frames.size()-1, true);
}

float THISSequence::getImageWidth()
{
	return imageWidth;
}

float THISSequence::getImageHeight()
{
	return imageHeight;
}

float THISSequence::getThumbWidth()
{
	return thumbWidth;
}

float THISSequence::getThumbHeight()
{
	return thumbHeight;
}

int THISSequence::getTotalFrames()
{
	return frames.size();
}

bool THISSequence::isLoaded()
{
	return loaded;
}

ofImage* THISSequence::getFrame(float percent, bool thumb)
{
	return getFrame(getIndexAtPercent(percent), thumb);
}

ofImage* THISSequence::getFrame(int frame, bool thumb)
{
	if(frame >= frames.size()){
		ofLog(OF_LOG_ERROR, "THISSequence -- accessing index %d when we only have %d frames. Returning last frame instead.", frame, frames.size());
		frame = frames.size()-1;
	}
	if(thumb){
		return frames[frame]->getThumbnail();
	}
	return frames[frame]->getFrame();
}

unsigned char* THISSequence::getPixelsForFrame(int frame, bool thumb)
{
	return getFrame(frame, thumb)->getPixels();
}

