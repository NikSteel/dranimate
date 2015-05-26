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
            
        case PUPPET_STAGE:
            
            
            
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
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'e' to export puppet", 300, 30);
            
            if(drawGui) meshGeneratedGui.draw();
            
            break;
            
        case PUPPET_STAGE:
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'l' to load a puppet", 300, 30);
            
            break;
            
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
            
            if(key == 's') {
             
                state = PUPPET_STAGE;
                
            }
            
            break;
            
        case IMAGE_SETTINGS:
            
            if(key == 'm') {
                generateMeshFromImage();
            }
            
            break;

        case MESH_GENERATED:
            
            if(key == 'e') {
                exportCurrentPuppet();
            }
            
            if(key == 'w') {
                drawWireframe = !drawWireframe;
            }
        
            break;
            
        case PUPPET_STAGE:
            
            if(key == 'l') {
                
                ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a puppet directory:",true);
                
                if (openFileResult.bSuccess){
                    loadPuppet(openFileResult.getPath());
                }
                
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
    
    cvImage.setFromPixels(newPuppet.noAlphaImage.getPixelsRef().getChannel(1));
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
    
    newPuppet.noAlphaImage = newPuppet.image;
    
    // replace alpha channel with white
    
    for(int x = 0; x < newPuppet.noAlphaImage.width; x++) {
        for(int y = 0; y < newPuppet.noAlphaImage.height; y++) {
            ofColor c = newPuppet.noAlphaImage.getColor(x, y);
            if(c.a == 0) {
                newPuppet.noAlphaImage.setColor(x, y, ofColor(255,255,255));
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
		if(m.getAddress() == "/aOSC/gyro/x") {
            gyroX = m.getArgAsFloat(0);
            gotX = true;
		}
        
        if(m.getAddress() == "/aOSC/gyro/y") {
            gyroY = m.getArgAsFloat(0);
            gotY = true;
		}
        
	}
    
    // temp code, testing osc
    
    if(gotX && gotY &&
       newPuppet.puppet.controlPointsVector.size() > 0) {
        int controlPointIndex = newPuppet.puppet.controlPointsVector[0];
        ofVec2f controlPointPosition = newPuppet.mesh.getVertex(controlPointIndex);
        newPuppet.puppet.setControlPoint(controlPointIndex,
                                         controlPointPosition + ofVec2f(gyroY,gyroX));
    }
    
}

void ofApp::exportCurrentPuppet() {
    
    ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
    
    if (saveFileResult.bSuccess){
        
        string path = saveFileResult.getPath();
        
        ofLog() << "making new directory at: " << path;
        
        // the folder itself
        string mkdirCommandString = "mkdir " + path;
        system(mkdirCommandString.c_str());
        
        // mesh
        newPuppet.mesh.save(path + "/mesh.ply");
        
        // image
        newPuppet.image.saveImage(path + "/image.png");
        
        // control points
        ofxXmlSettings controlPoints;
        controlPoints.addTag("controlPoints");
        controlPoints.pushTag("controlPoints");
        for(int i = 0; i < newPuppet.puppet.controlPointsVector.size(); i++){
            
            controlPoints.addTag("controlPoint");
            controlPoints.pushTag("controlPoint",i);
            
            controlPoints.addValue("index", newPuppet.puppet.controlPointsVector[i]);
            controlPoints.popTag();
            
        }
        controlPoints.popTag();
        controlPoints.saveFile(path + "/controlPoints.xml");
        
        
        // todo: save matrices that svd calculates to allow near-instant loading of puppets
        
    }
    
}

void ofApp::loadPuppet(string path) {
    
    // load image
    newPuppet.image.loadImage(path + "/image.png");
    newPuppet.mesh.load(path + "/mesh.ply");
    
    
    // load mesh
    newPuppet.puppet.setup(newPuppet.mesh);
    newPuppet.updateSubdivisionMesh();
    
    // load control points & osc settings
    ofxXmlSettings controlPoints;
    if(controlPoints.loadFile(path + "/controlPoints.xml")){
        
        controlPoints.pushTag("controlPoints");
        int nControlPoints = controlPoints.getNumTags("controlPoint");
        
        for(int i = 0; i < nControlPoints; i++){
            
            controlPoints.pushTag("controlPoint", i);
            
            int controlPointIndex = controlPoints.getValue("index", 0);
            newPuppet.puppet.setControlPoint(controlPointIndex);
            
            controlPoints.popTag();
            
        }
        
        controlPoints.popTag();
    }
    
    state = MESH_GENERATED;

}

// lets us drag an image/puppet directory into the window to load it - very convenient

void ofApp::dragEvent(ofDragInfo info) {
    
    switch (state) {
            
        case LOAD_IMAGE:
            if(info.files.size() > 0) {
                loadAndCleanupImage(info.files.at(0));
            }
            break;
            
        case IMAGE_SETTINGS:
            break;
            
        case MESH_GENERATED:
            break;
            
        case PUPPET_STAGE:
            if(info.files.size() > 0) {
                loadPuppet(info.files.at(0));
            }
            break;
            
        default:
            break;
            
    }
    
}
