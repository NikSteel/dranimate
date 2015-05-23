#include "ofApp.h"

void ofApp::setup() {
    
    drawGui = true;
    
    imageSettingsGui.setup();
    imageSettingsGui.add(imageThreshold.setup("image threshold", 254, 0, 255));
    imageSettingsGui.add(invert.setup("invert", true));
    
    meshGeneratedGui.setup();
    meshGeneratedGui.add(drawWireframe.setup("draw wireframe", true));
    
    state = LOAD_IMAGE;
    
    receiver.setup(8000);
    
}

void ofApp::update() {
    
    ofShowCursor();
    
    switch(state) {
            
        case LOAD_IMAGE:
            break;
            
        case IMAGE_SETTINGS:
            
            findImageContours();
            
            break;
            
        case MESH_GENERATED:
            
            newPuppet.update();
            
            recieveOsc();
            
            break;
            
    }
    
}

void ofApp::draw() {
    
    ofSetColor(255);
    ofBackground(0);
    
    switch(state) {
            
        case LOAD_IMAGE:
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Drag file into window or press 'l' to load an image", 300, 30);
            
            break;
    
        case IMAGE_SETTINGS:
        
            // draw thresholded image
            
            ofSetColor(255,255,255);
            cvImage.draw(0, 0);
            
            // draw contours found from the thresholded image
            
            ofSetColor(255, 0, 0);
            contourFinder.draw();
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'm' to generate mesh when ready", 300, 30);
        
            if(drawGui) imageSettingsGui.draw();
            
            break;
        
        case MESH_GENERATED:
            
            newPuppet.draw(drawWireframe);
            
            if(drawGui) meshGeneratedGui.draw();
            
    }
    
}

void ofApp::keyReleased(int key) {
    
    switch (state) {
            
        case LOAD_IMAGE:
            
            if(key == 'l') {
                
                ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image:",true);
                
                if (openFileResult.bSuccess){
                    loadAndCleanupImage(openFileResult.getPath());
                }
                
            }
            
            break;
            
        case IMAGE_SETTINGS:
            
            if(key == 'm') {
                generateMeshFromImage();
            }
            
            break;

        case MESH_GENERATED:
            
            if(key == 'e') {
                
                ofFileDialogResult saveFileResult = ofSystemSaveDialog("mesh.ply", "where to save mesh?");
                if (saveFileResult.bSuccess){
                    ofLog() << saveFileResult.getPath();
                }
                
                //mesh.save(imageFilename+"-mesh.ply");
            }
        
            break;
            
        default:
            break;
            
    }
    
    if(key == 'g') {
        drawGui = !drawGui;
    }
    
}

void ofApp::findImageContours() {
    
    // threshold image
    
    cvImage.setFromPixels(newPuppet.image.getPixelsRef().getChannel(1));
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

void ofApp::generateMeshFromImage() {
    
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
            triangleMesh.triangulate(lineRespaced, 20, -1);
            
        }
    }
    
    newPuppet.mesh = triangleMesh.triangulatedMesh;

    // reset mesh texture coords to match with the image
    
    int len = newPuppet.mesh.getNumVertices();
    for(int i = 0; i < len; i++) {
        ofVec2f vec = newPuppet.mesh.getVertex(i);
        newPuppet.mesh.addTexCoord(vec);
    }
    
    // setup puppet
    
    newPuppet.puppet.setup(newPuppet.mesh);
    
    newPuppet.updateSubdivisionMesh();

    state = MESH_GENERATED;
    
}

void ofApp::loadAndCleanupImage(string fn) {
    
    newPuppet.image.loadImage(fn);
    
    // scale down image to a good size for the mesh generator
    
    float whRatio = (float)newPuppet.image.width/(float)newPuppet.image.height;
    if(newPuppet.image.width > newPuppet.image.height) {
        newPuppet.image.resize(IMAGE_BASE_SIZE*whRatio, IMAGE_BASE_SIZE);
    } else {
        whRatio = (float)newPuppet.image.height/(float)newPuppet.image.width;
        newPuppet.image.resize(IMAGE_BASE_SIZE, IMAGE_BASE_SIZE*whRatio);
    }
    
    // replace alpha channel with white
    
    for(int x = 0; x < newPuppet.image.width; x++) {
        for(int y = 0; y < newPuppet.image.height; y++) {
            ofColor c = newPuppet.image.getColor(x, y);
            if(c.a == 0) {
                newPuppet.image.setColor(x, y, ofColor(255,255,255));
            }
        }
    }
    
    state = IMAGE_SETTINGS;
    
}

void ofApp::recieveOsc() {
    
    //http://talk.olab.io/t/osc-communication-between-maxmsp-and-openframeworks/121/3
    
    // testing controlling control points with osc
    
    float gyroX = 0;
    float gyroY = 0;
    
    bool gotX = false;
    bool gotY = false;
    
    // check for waiting messages
	while(receiver.hasWaitingMessages()) {
        
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
		// check for gryo message
		if(m.getAddress() == "/aOSC/gyro/x"){
            gyroX = m.getArgAsFloat(0);
            gotX = true;
		}
        
        if(m.getAddress() == "/aOSC/gyro/y"){
            gyroY = m.getArgAsFloat(0);
            gotY = true;
		}
        
	}
    
    /*
    if(gotX && gotY &&
       puppet.controlPointsVector.size() > 0) {
        int controlPointIndex = puppet.controlPointsVector[0];
        ofVec2f controlPointPosition = mesh.getVertex(controlPointIndex);
        puppet.setControlPoint(controlPointIndex,
                               controlPointPosition + ofVec2f(gyroY,gyroX));
    }
     */
    
}

void ofApp::saveCurrentPuppet() {
    
    
    
}

// lets us drag an image into the window to load it - very convenient

void ofApp::dragEvent(ofDragInfo info) {
    
    if(info.files.size() > 0) {
        loadAndCleanupImage(info.files.at(0));
    }
    
}
