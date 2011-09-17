/*
 *  THISSequence.h
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#ifndef THIS_SEQUENCE
#define THIS_SEQUENCE

#include "ofMain.h"
#include "THISFrame.h"

static GLint glTypeForImageType(int imageType){
	if(imageType == OF_IMAGE_GRAYSCALE) return GL_LUMINANCE;
	if(imageType == OF_IMAGE_COLOR) return GL_RGB;
	if(imageType == OF_IMAGE_COLOR_ALPHA) return GL_RGBA;
}

typedef struct
{
	ofTexture* texture;
	ofRectangle bounds;
	int frameIndex;
} PreviewTexture;

class THISSequence 
{
  public:
	THISSequence();
	~THISSequence();

	void loadSequence();
	void loadSequence(string directoryName);
    void clear();
	
    void purgeFrames();
    void purgeThumbs();
	
	void setDrawRange(ofRange newRange);
	void setDrawWidth(float width);
	void setImageType(int type);
	int getTotalFrames();
	
	void draw(int x, int y);
	
	
    float heightPerFrame;
    
	float getImageWidth();
	float getImageHeight();
	
	float getThumbWidth();
	float getThumbHeight();
	
	int getIndexAtPercent(float percent);
	
    ofImage* getFrame(float percent, bool thumb);
	ofImage* getFrame(int frame, bool thumb);
    
	unsigned char* getPixelsForFrame(int frame, bool thumb);
	
    bool isLoaded();
    string directory;
    
	vector<PreviewTexture> previewTextures;

  protected:	
	
	void recomputePreview();
	
	void clearPreviewTextures();
	void clearFrames();
	
    string pathToDirectory;
    
	bool loaded;
	bool widthSet;
	bool viewIsDirty;
	
	int imageType;
    
	float imageWidth, imageHeight;
	float thumbWidth, thumbHeight;
	
	ofRange drawRange;
	float drawWidth;
	int framesToShow;
	float widthPerFrame;
	vector<THISFrame*> frames;	
};

#endif