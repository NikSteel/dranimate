#include "MeshGenerator.h"

void MeshGenerator::setup() {
    
    gui.setup();
    gui.add(imageThreshold.setup("image threshold", 254, 0, 255));
    gui.add(invert.setup("invert", true));
    
}

void MeshGenerator::update() {
    
    findImageContours();
    
}

void MeshGenerator::draw() {
    
    ofSetColor(255,255,255);
    cvImage.draw(0, 0);
    
    // draw contours found from the thresholded image
    
    ofSetColor(255, 0, 0);
    contourFinder.draw();
    
    gui.draw();
    
}

void MeshGenerator::findImageContours() {
    
    // threshold image
    
    cvImage.setFromPixels(noAlphaImage.getPixelsRef().getChannel(1));
    cvImage.threshold(imageThreshold);
    if(invert) cvImage.invert();
    
    // find contours from thresholded image
    
    contourFinder.setMinArea(1000);
    contourFinder.setMaxArea(640*480);
    //contourFinder.setFindHoles(true);
    contourFinder.setSortBySize(true);
    
    //contourFinder.setThreshold(100);
    contourFinder.findContours(ofxCv::toCv(cvImage));
    
}

void MeshGenerator::setImage(ofImage img) {
    
    noAlphaImage = img;
    
    // replace alpha channel with white
    
    for(int x = 0; x < noAlphaImage.width; x++) {
        for(int y = 0; y < noAlphaImage.height; y++) {
            ofColor c = noAlphaImage.getColor(x, y);
            if(c.a == 0) {
                noAlphaImage.setColor(x, y, ofColor(255,255,255));
            }
        }
    }
    
}

ofMesh MeshGenerator::generateMesh() {
    
    // create a polyline with all of the contour points
    
    vector<cv::Point> contour = contourFinder.getContour(0);
    for(int i = 0; i < contour.size(); i++) {
        line.addVertex(contour[i].x,contour[i].y);
    }
    
    // use that polyline to generate a mesh with ofxTriangleMesh !!
    // (code from ofxTriangleMesh example)
    
    if (line.size() > 2){
        
        ofPolyline lineRespaced = line;
        
        // add the last point (so when we resample, it's a closed polygon)
        lineRespaced.addVertex(lineRespaced[0]);
        
        // resample
        lineRespaced = lineRespaced.getResampledBySpacing(20);
        
        // I want to make sure the first point and the last point are not the same, since triangle is unhappy:
        lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
        
        // if we have a proper set of points, mesh them:
        if (lineRespaced.size() > 5){
            
            // angle constraint = 28
            // size constraint = -1 (don't constraint triangles by size);
            triangleMesh.triangulate(lineRespaced, 28, -1);
            
        }
    }
    
    ofMesh mesh = triangleMesh.triangulatedMesh;
    
    // reset mesh texture coords to match with an image
    
    int len = mesh.getNumVertices();
    for(int i = 0; i < len; i++) {
        ofVec2f vec = mesh.getVertex(i);
        mesh.addTexCoord(vec);
    }
    
    return mesh;
    
}