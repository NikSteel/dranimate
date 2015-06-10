#include "PuppetRecorder.h"

void PuppetRecorder::setup() {
    
    // reset everything
    imageLoaded = false;
    animationFrames.clear();
    currentFrame = 0;
    
    // todo: add toggle
    isPlaying = true;
    
}


void PuppetRecorder::recordPuppetFrame(Puppet puppet) {
    
    // if we haven't copied the puppet base image over, get it now
    if(!imageLoaded) {
        image = puppet.image;
    }
    
    // add a 'frame' (which is really just the mesh of a puppet at a point in time)
    animationFrames.push_back(puppet.subdivided);
    
}

void PuppetRecorder::nextFrame() {
    
    currentFrame++;
    
    // if we reached the end of the animation, reset it.
    if(currentFrame >= animationFrames.size()) {
        currentFrame = 0;
    }
    
}

void PuppetRecorder::save(string path) {
    
    // make a new dir for the recording
    string mkdirCommandString = "mkdir " + path;
    system(mkdirCommandString.c_str());
    
    for(int i = 0; i < animationFrames.size(); i++) {
        animationFrames[i].save(path + "/frame" + ofToString(i) + ".ply");
    }
    
    image.saveImage(path + "/image.png");
    
    ofLog() << "recording saved to " << path;
    
}

void PuppetRecorder::load(string path) {
    
    // todo
    
}

bool PuppetRecorder::isEmpty() {
    
    return animationFrames.size() == 0;
    
}

void PuppetRecorder::update() {
    
    if(isPlaying) nextFrame();
    
}

void PuppetRecorder::draw() {
    
    if(!isEmpty()) {
        image.bind();
        animationFrames[currentFrame].drawFaces();
        image.unbind();
    }
    
    
}