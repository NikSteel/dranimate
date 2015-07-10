#include "PuppetRecorder.h"

void PuppetRecorder::setup() {
    
    // reset everything
    imageLoaded = false;
    animationFrames.clear();
    currentFrame = 0;
    
    recorder.allocate(ofGetWidth(), ofGetHeight());
    
    playingForwards = true;
    
}


void PuppetRecorder::recordPuppetFrame(Puppet *puppet) {
    
    // if we haven't copied the puppet base image over, get it now
    if(!imageLoaded) {
        //image = puppet->image;
    }
    
    // add a 'frame' (which is really just the mesh of a puppet at a point in time)
    //animationFrames.push_back(puppet->subdivided);
    
}

void PuppetRecorder::nextFrame() {
    
    if(playingForwards) {
    
        currentFrame++;
        
        if(currentFrame >= animationFrames.size()) {
            playingForwards = false;
            currentFrame = animationFrames.size()-1;
        }
        
    } else {
        
        currentFrame--;
        
        if(currentFrame < 0) {
            playingForwards = true;
            currentFrame = 0;
        }
        
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

void PuppetRecorder::exportAsMovie() {
    
    string movieName = ofGetTimestampString();
    
    currentFrame = 0;
    
    for(int i = 0; i < animationFrames.size(); i++) {
        currentFrame = i;
        
        // draw puppet to fbo
        recorder.begin();
        ofSetColor(0, 0, 0);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
        ofSetColor(255, 255, 255);
        draw();
        recorder.end();
        
        // save what we drew in the fbo to an ofimage
        ofImage img;
        ofPixels p;
        recorder.readToPixels(p);
        img.setFromPixels(p);
        img.saveImage("movies/"+movieName+"/frames/frame"+ofToString(currentFrame)+".png");
        
        ofLog() << "saved frame " << ofToString(i) << " of " << ofToString(animationFrames.size());
    }
    
    // convert images to movie
    ofLog() << "creating movie from frames...";
    ofSystem("./../../../data/resources/ffmpeg -framerate 60 -i ../../../data/movies/"+movieName+"/frames/frame%d.png -c:v libx264 -r 30 -pix_fmt yuv420p ../../../data/movies/"+movieName+"/movie.mov");
    
}

void PuppetRecorder::load(string path) {
    
    // todo
    
}

bool PuppetRecorder::isEmpty() {
    
    return animationFrames.size() == 0;
    
}

ofMesh PuppetRecorder::getCurrentMesh() {
    
    return animationFrames[currentFrame];
    
}

void PuppetRecorder::update() {
    
    if(!isPaused) nextFrame();
    
}

void PuppetRecorder::draw() {
    
    if(!isEmpty()) {
        
        ofSetColor(255,255,255,255);
        
        image.bind();
        glEnable(GL_DEPTH_TEST);
        animationFrames[currentFrame].drawFaces();
        glDisable(GL_DEPTH_TEST);
        image.unbind();
        
    }
    
}