//
//  ofxSpeech.h
//  emptyExample
//
//  Created by Timothy Gfrerer on 28/01/2011.
//  Copyright 2011. All rights reserved.
//

#ifndef GUARD_OFXFILEDIALOGCOCOA
#define GUARD_OFXFILEDIALOGCOCOA

#include "ofMain.h"

class ofxFileDialogCocoa {
  public:
  
	ofxFileDialogCocoa();
	~ofxFileDialogCocoa();
  
    bool open(const string& _initPath, const string& _filter, string& _filename, string& _fileAbsolutePath);
	bool canChooseDirectory;
    bool canChooseFile;
    bool allowMultiple;
    
  	void * OSXFileDialog;
  
};




#endif