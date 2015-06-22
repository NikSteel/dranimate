#include "MeshGenerator.h"

void MeshGenerator::setup() {
    
    gui.setup();
    gui.add(imageThreshold.setup("image threshold", 254, 0, 255));
    gui.add(invertImage.setup("invert image", true));
    gui.add(contourResampleAmt.setup("contour resample amount", 15, 1, 30));
    gui.add(triangleAngleConstraint.setup("triangle angle constraint", 28, 0, 28));
    gui.add(triangleSizeConstraint.setup("triangle size constraint", -1, -1, 100));
    
    meshGenerated = false;
    
}

void MeshGenerator::update() {
    
    findImageContours();
    
}

void MeshGenerator::draw() {
    
    // draw image
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2  - cvImage.width/2,
                ofGetHeight()/2 - cvImage.height/2);
    
    if(!meshGenerated) {
        ofSetColor(255,255,255,255);
    } else {
        ofSetColor(100,100,100,255);
    }
    noAlphaImage.draw(0,0);
    
    // draw mesh
    
    if(meshGenerated) {
        ofSetColor(0, 255, 0);
        mesh.drawWireframe();
    }
    
    // draw user-added extra verts
    
    for(int i = 0; i < extraVerts.size(); i++) {
        int blinkColor = round(sin(ofGetElapsedTimef())+1)*255/2;
        ofSetColor(blinkColor, 0, 0);
        ofCircle(extraVerts[i].x, extraVerts[i].y, 2);
    }
    
    // draw contours found from the thresholded image
    
    if(!meshGenerated) {
        ofSetColor(255, 0, 0);
        contourFinder.draw();
    }
    
    ofPopMatrix();
    
    gui.draw();
    
}

void MeshGenerator::reset() {
    
    meshGenerated = false;
    contourLine.clear();
    extraVerts.clear();
    
}

void MeshGenerator::findImageContours() {
    
    // threshold image
    
    cvImage.setFromPixels(noAlphaImage.getPixelsRef().getChannel(1));
    cvImage.threshold(imageThreshold);
    if(invertImage) cvImage.invert();
    
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

void MeshGenerator::addExtraVertex(int x, int y) {
    
    int nx = x - (ofGetWidth()/2  - cvImage.width/2);
    int ny = y - (ofGetHeight()/2 - cvImage.height/2);
    
    //temporarily disabled
    //extraVerts.push_back(ofPoint(nx,ny));
    
}

void MeshGenerator::generateMesh() {
    
    // create a polyline with all of the contour points
    
    contourLine.clear();
    
    vector<cv::Point> contour = contourFinder.getContour(0);
    for(int i = 0; i < contour.size(); i++) {
        contourLine.addVertex(contour[i].x,contour[i].y);
    }
    
    // use that polyline to generate a mesh with ofxTriangleMesh !!
    // (code from ofxTriangleMesh example)
    
    if (contourLine.size() > 2){
        
        ofPolyline lineRespaced = contourLine;
        
        // add the last point (so when we resample, it's a closed polygon)
        lineRespaced.addVertex(lineRespaced[0]);
        
        // resample
        lineRespaced = lineRespaced.getResampledBySpacing(contourResampleAmt);
        
        // I want to make sure the first point and the last point are not the same, since triangle is unhappy:
        lineRespaced.getVertices().erase(lineRespaced.getVertices().begin());
        
        for(int i = 0; i < extraVerts.size(); i++) {
            lineRespaced.addVertex(extraVerts[i]);
        }
        
        // if we have a proper set of points, mesh them:
        if (lineRespaced.size() > 5){
            
            // note: size constraint = -1 means don't constrain by size
            triangleMesh.triangulate(lineRespaced, triangleAngleConstraint, triangleSizeConstraint);
            
        }
    }
    
    mesh = triangleMesh.triangulatedMesh;
    
    // fix loosely connected faces
    // i.e., if two triangles are connected by one point.
    // (this causes ofxPuppet to freak out.)
    
    //todo
    
    // somehow fix broken meshes
    // i.e., there exist whole pieces of the mesh that are
    // completely disconnected from the rest of the mesh (islands)
    // (this causes ofxPuppet to disappear or even crash)
    
    //todo
    
    // reset mesh texture coords to match with an image
    
    int len = mesh.getNumVertices();
    for(int i = 0; i < len; i++) {
        ofVec2f vec = mesh.getVertex(i);
        mesh.addTexCoord(vec);
    }
    
    meshGenerated = true;
    
}

ofMesh MeshGenerator::getMesh() {
    
    return mesh;
    
}

ofPolyline MeshGenerator::getContour() {
    
    return contourLine;
    
}