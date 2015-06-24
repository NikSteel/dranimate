#include "SceneRecorder.h"

void SceneRecorder::setup() {
    
    frames.clear();
    
}

void SceneRecorder::update() {
    
    RecordedFrame newFrame;
    frames.push_back(newFrame);
    
}

void SceneRecorder::addPuppetToFrame(PuppetRecorder puppet) {
    
    frames[frames.size()-1].images.push_back(puppet.image);
    frames[frames.size()-1].meshes.push_back(puppet.getCurrentMesh());
    
}

void SceneRecorder::exportAsMovie() {
    
    string movieName = ofGetTimestampString();
    ofSystem("mkdir ../../../data/movies/" + movieName);
    ofSystem("mkdir ../../../data/movies/" + movieName + "/frames");
    
    exportFbo.allocate(ofGetWidth(), ofGetHeight());
    
    for(int i = 0; i < frames.size(); i++) {
        
        exportFbo.begin();
        
        // clear the fbo so we don't draw any vram garbage memory
        ofSetColor(0, 0, 0, 0);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
        ofClear(ofColor(0,0,0,0));
        ofSetColor(255, 255, 255);
        
        // draw each puppet in the frame
        for(int m = 0; m < frames[i].meshes.size(); m++) {
            
            frames[i].images[m].bind();
            frames[i].meshes[m].drawFaces();
            
        }
        
        exportFbo.end();
        
        // save what we drew in the fbo to an ofimage
        ofImage img;
        ofPixels p;
        exportFbo.readToPixels(p);
        img.setFromPixels(p);
        img.saveImage("movies/"+movieName+"/frames/frame"+ofToString(i)+".png");
        
        ofLog() << "saved frame " << ofToString(i+1) << " of " << ofToString(frames.size());
        
    }
    
    // convert images to movie
    ofLog() << "creating movie from frames in "+movieName+"...";
    ofSystem("./../../../data/resources/ffmpeg -framerate 60 -i ../../../data/movies/"+movieName+"/frames/frame%d.png -c:v libx264 -r 30 -pix_fmt yuv420p ../../../data/movies/"+movieName+"/movie.mov");
    ofSystem("./../../../data/resources/ffmpeg -vcodec png -framerate 60 -i ../../../data/movies/"+movieName+"/frames/frame%d.png -c:v libx264 -r 30 -pix_fmt yuv420p ../../../data/movies/"+movieName+"/movie-transparency.mov");
    
}