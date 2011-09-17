/*
 *  THISFrame.h
 *  THIS_Editor
 *
 *  Created by Jim on 9/23/10.
 *  Copyright 2010 FlightPhase. All rights reserved.
 *
 */

#ifndef THIS_FRAME
#define THIS_FRAME

#include "ofMain.h"

class THISFrame
{
  public:
	THISFrame();
	~THISFrame();
	
	string filename;
    string thumbFilename;
    string shortFilename;
	float lastUsedTime;
	
	void setType(int type);
	
	void setFrame(string filename);
	ofImage* getFrame();
	ofImage* getThumbnail();
	
	void setDesiredThumbnailWidth(int width);
	
    int getFullFrameWidth();
    int getFullFrameHeight();

    int getThumbWidth();
    int getThumbHeight();

    bool loadThumb();
	bool loadFrame();
    bool isFrameLoaded();
    bool isThumbLoaded();

	void clear();

  protected:
	bool generateThumbnail();
	
	int desiredThumbWidth;
	
    int frameWidth;
    int frameHeight;
    int thumbWidth;
    int thumbHeight;
    
	bool frameLoaded;
	bool thumbLoaded;
	
	ofImage* frame;
	ofImage* thumbnail;
	int type;
	
};

#endif;