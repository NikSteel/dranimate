#ifndef __dranimate__SceneRecorder__
#define __dranimate__SceneRecorder__

#include <stdio.h>
#include <vector>

#include "ofMain.h"
#include "PuppetRecorder.h"

class RecordedFrame {
    
public:
    
    vector<ofMesh> meshes;
    vector<ofImage> images;
    
};

class SceneRecorder {
    
public:
    
    vector<RecordedFrame> frames;
    
    ofFbo exportFbo;
    
    void setup();
    void update();
    
    void addPuppetToFrame(PuppetRecorder puppet);
    void exportAsMovie();
    
};

#endif