#ifndef __dranimate__MeshGenerator__
#define __dranimate__MeshGenerator__

#include <iostream>
#include <vector>

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxTriangleMesh.h"
#include "ofxGui.h"
#include "Utils.h"

class MeshGenerator {
    
private:
    
    void findImageContours();
    
    ofMesh mesh;
    
    ofPolyline contourLine;
    ofxTriangleMesh triangleMesh;
    
    ofImage image;
    ofImage noAlphaImage;
    ofxCvGrayscaleImage cvImage;
    ofxCv::ContourFinder contourFinder;
    
    std::vector<ofPoint> extraVerts;
    
    ofxPanel gui;
    
    ofxSlider<int> rotation;
    ofxToggle flipVertical;
    ofxToggle flipHorizontal;
    ofxSlider<int> imageThreshold;
    ofxToggle invertImage;
    
    ofxSlider<int> contourResampleAmt;
    ofxSlider<int> triangleAngleConstraint;
    ofxSlider<int> triangleSizeConstraint;
    
    bool isMeshBroken();
    
    bool meshGenerated;
    
public:
    
    void setup();
    void update();
    void draw();
    void reset();
    
    void setImage(ofImage img);
    void addExtraVertex(int x, int y);
    
    void generateMesh();
    
    ofImage addAlphaToImage(ofImage img);
    
    ofImage getImage();
    ofMesh getMesh();
    ofPolyline getContour();
    
};

#endif
