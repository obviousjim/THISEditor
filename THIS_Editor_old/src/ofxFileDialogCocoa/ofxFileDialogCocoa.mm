//
//  ofxFileDialogCocoa.m
//
//  Created by Timothy Gfrerer on 28/01/2011.
//  Copyright 2011. All rights reserved.
//

#import "ofxFileDialogCocoa.h"
#import <Cocoa/Cocoa.h>


@interface  OSXFileDialogCocoa: NSObject {}

- (void) dealloc;

@end

// ******************************************************************************** //


@implementation OSXFileDialogCocoa

-(void) dealloc{
  [super dealloc];
}

@end

// ******************************************************************************** //

ofxFileDialogCocoa::ofxFileDialogCocoa(){
  	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  	OSXFileDialog = [[OSXFileDialogCocoa alloc] init];
  
	//no need to retain on alloc/init objects
	//[(OSXFileDialogCocoa*)OSXFileDialog retain];
	canChooseDirectory = false;	
    canChooseFile = true;
    allowMultiple = false;
    
	//jg added good ol' Cocoa clean up ;)
	[pool release];
}

bool ofxFileDialogCocoa::open(const string& _initPath, const string& _filter, string& _filename, string& _fileAbsolutePath){
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  NSOpenPanel* FileOpenDialog = [NSOpenPanel openPanel] ;
  [FileOpenDialog setCanChooseFiles:canChooseFile];
  [FileOpenDialog setCanChooseDirectories:canChooseDirectory];
  [FileOpenDialog setAllowsMultipleSelection:allowMultiple];
  //JG: we want Cocoa to manage defaults so it saves where the user last opened a file
 // [FileOpenDialog setDirectoryURL:[[NSURL alloc] initFileURLWithPath:[[NSString alloc] initWithCString:_initPath.c_str()] isDirectory:YES]];
  [FileOpenDialog setAllowedFileTypes:[[NSArray alloc] initWithObjects:[[NSString alloc] initWithCString:_filter.c_str()],nil]];
   
  bool success = [FileOpenDialog runModal]== NSOKButton;
  if (success){
    NSArray* urls = [FileOpenDialog URLs];
    NSString * SelectedUrl = [[urls objectAtIndex:0] relativePath];

    _filename = [SelectedUrl UTF8String];
    _fileAbsolutePath = [[SelectedUrl stringByDeletingLastPathComponent] UTF8String];
  }
  
  //jg added good ol' Cocoa clean up ;)
  [pool release];
  return success;
	
  
}

ofxFileDialogCocoa::~ofxFileDialogCocoa()
{
  //all cocoa operations need to be wrapped in release pools or we'll leak
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

  //little cocoa bit, never call dealloc directly always call 'release' to say we are done.
  //which will cause dealloc to be called when necessary (since cocoa uses reference counting)
  [(OSXFileDialogCocoa*)OSXFileDialog release];
  
  [pool release];  
}

