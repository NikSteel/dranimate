#ifndef __dranimate__MeshGenerator__
#define __dranimate__MeshGenerator__

#include <iostream>
#include <vector>

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
    
    std::vector<ofPoint> extraVerts;
    
    ofxPanel gui;
    ofxSlider<int> imageThreshold;
    ofxToggle invertImage;
    ofxSlider<int> contourResampleAmt;
    ofxSlider<int> triangleAngleConstraint;
    ofxSlider<int> triangleSizeConstraint;
    
    bool meshGenerated;
    
public:
    
    void setup();
    void update();
    void draw();
    void reset();
    
    void setImage(ofImage img);
    void addExtraVertex(int x, int y);
    
    void generateMesh();
    
    ofMesh getMesh();
    ofPolyline getContour();
    
};

#endif
