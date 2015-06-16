#ifndef __dranimate__MeshGenerator__
#define __dranimate__MeshGenerator__

#include <iostream>

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxTriangleMesh.h"
#include "ofxGui.h"

class MeshGenerator {
    
private:
    
    void findImageContours();
    
    ofMesh mesh;
    
    ofPolyline contourLine;
    ofxTriangleMesh triangleMesh;
    
    ofImage noAlphaImage;
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
    ofxPanel gui;
    ofxSlider<int> imageThreshold;
    ofxToggle invertImage;
    ofxSlider<int> contourResampleAmt;
    ofxSlider<int> triangleAngleConstraint;
    ofxSlider<int> triangleSizeConstraint;
    
public:
    
    void setup();
    void draw();
    void update();
    
    void setImage(ofImage img);
    
    void generateMesh();
    
    ofMesh getMesh();
    ofPolyline getContour();
    
};

#endif
