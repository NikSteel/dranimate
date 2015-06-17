#ifndef __dranimate__PuppetRecorder__
#define __dranimate__PuppetRecorder__

#include <stdio.h>
#include <vector>

#include "ofMain.h"
#include "Puppet.h"

class PuppetRecorder {
    
public:
    
    void setup();
    void update();
    void draw();
    
    void save(string path);
    void load(string path);
    
    void exportAsMovie();
    
    void recordPuppetFrame(Puppet *puppet);
    void nextFrame();
    bool isEmpty();
    
    bool imageLoaded;
    ofImage image;
    
    std::vector<ofMesh> animationFrames;
    
    bool isPaused;
    int currentFrame;
    
    ofFbo recorder;
    
};

#endif
