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
            
            // todo: update all puppets and recieve osc
            
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
            
            // todo: draw all of the currently loaded puppets.
            
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
                
                MeshGenerator mesher;
                newPuppet.setMesh(mesher.generateMesh(contourFinder));
                
                state = MESH_GENERATED;
                
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
        
        // loop through all osc namespaces in each control point
        // ->holy moly! fix this code up!
        
        for(int i = 0; i < newPuppet.controlPoints.size(); i++) {
            
            ControlPoint controlPoint = newPuppet.controlPoints[i];
            vector<OSCNamespace> namespaces = controlPoint.oscNamespaces;
            
            for(int j = 0; j < namespaces.size(); j++) {
                
                OSCNamespace namesp = namespaces[j];
                
                if(namesp.message == m.getAddress()) {
                    
                    ofLog() << "match for " << m.getAddress();
                    
                    float value = m.getArgAsFloat(0);
                    
                    if(namesp.controlType == "x") {
                        
                    } else if(namesp.controlType == "y") {
                        ofLog() << "match for " << namesp.controlType;
                        
                        int controlPointIndex = controlPoint.index;
                        ofVec2f controlPointPosition = newPuppet.mesh.getVertex(controlPointIndex);
                        controlPointPosition.x += value;
                        newPuppet.puppet.setControlPoint(controlPointIndex,
                                                         controlPointPosition);
                    }
                
                }
                
            }
            
        }
        
	}
    
}

void ofApp::exportCurrentPuppet() {
    
    ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
    
    if (saveFileResult.bSuccess){
        
        string path = saveFileResult.getPath();
        
        newPuppet.save(path);
        
    }
    
}

void ofApp::loadPuppet(string path) {
    
    newPuppet.load(path);
    
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
