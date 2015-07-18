#ifndef __dranimate__MeshGenerator__
#define __dranimate__MeshGenerator__

#include <iostream>
#include <vector>

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxTriangleMesh.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

#include "Utils.h"
#include "ImageFromCamera.h"
#include "Global.h"

class MeshGenerator {
    
public:
    
    void setup();
    void update();
    void draw();
    
    void reset(bool liveMode);
    
    void setImage(ofImage img);
    void addExtraVertex(int x, int y);
    
    void generateMesh();
    
    void saveXMLSettings();
    
    ofImage getImage();
    ofMesh getMesh();
    
private:
    
    const static int IMAGE_BASE_SIZE = 800;
    
    void findImageContours();
    
    bool isMeshBroken();
    
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
    ofxToggle invertImage;
    
    ofxSlider<int> imageThreshold;
    ofxToggle useAdaptiveThreshold;
    ofxToggle invertThresholdImage;
    
    ofxSlider<int> contourResampleAmt;
    ofxSlider<int> triangleAngleConstraint;
    ofxSlider<int> triangleSizeConstraint;
    
    bool meshGenerated;
    
    ImageFromCamera cam;
    
    enum ImageType {
        FROM_FILE,
        FROM_LIFE_FEED
    };
    ImageType imageType;
    
    ofImage grid;
    
};

#endif
