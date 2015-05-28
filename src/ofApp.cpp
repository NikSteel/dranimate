#include "ofApp.h"

void ofApp::setup() {
    
    receiver.setup(8000);
    leap.open();
    
    mesher.setup();
    
    state = PUPPET_STAGE;
    
}

void ofApp::update() {
    
    ofShowCursor();
    
    switch(state) {
            
        case LOAD_IMAGE:
            break;
            
        case IMAGE_SETTINGS:
            mesher.update();
            break;
            
        case MESH_GENERATED:
            recieveOsc();
            recieveLeap();
            newPuppet.update();
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
            ofDrawBitmapString("Create a puppet:\nDrag file into window or press 'l' to load an image", 300, 30);
            
            break;
    
        case IMAGE_SETTINGS:
        
            mesher.draw();
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Adjust image settings and press 'm' to generate mesh when ready", 300, 30);
            
            break;
        
        case MESH_GENERATED:
            
            newPuppet.draw(drawWireframe);
            
            ofPushStyle();
            ofNoFill();
            ofSetColor(ofColor(0,155,255));
            ofCircle(selectedVertexPosition, 5);
            ofPopStyle();
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'e' to export current puppet", 300, 30);
            
            break;
            
        case PUPPET_STAGE:
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'l' to load a puppet\nOr, press 's' to create a new puppet", 300, 30);
            
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
                    newPuppet.setImage(openFileResult.getPath());
                    mesher.setImage(newPuppet.image);
                    state = IMAGE_SETTINGS;
                }
                
            }
            
            break;
            
        case IMAGE_SETTINGS:
            
            if(key == 'm') {
                
                newPuppet.setMesh(mesher.generateMesh());
                
                state = MESH_GENERATED;
                
            }
            
            break;

        case MESH_GENERATED:
            
            if(key == 'e') {
                
                ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
                
                if (saveFileResult.bSuccess){
                    string path = saveFileResult.getPath();
                    newPuppet.save(path);
                }
                
            }
            
            if(key == 'w') {
                drawWireframe = !drawWireframe;
            }
        
            break;
            
        case PUPPET_STAGE:
            
            if(key == 'l') {
                
                ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a puppet directory:",true);
                
                if (openFileResult.bSuccess){
                    newPuppet.load(openFileResult.getPath());
                    state = MESH_GENERATED;
                }
                
            }
            
            if(key == 's') {
                
                state = LOAD_IMAGE;
                
            }
            
            break;
            
        default:
            break;
            
    }
    
}

void ofApp::recieveLeap() {
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-230, 230, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-150, 150, -200, 200);
        
        for(int i = 0; i < simpleHands.size(); i++){
            
            for(int j = 0; j < simpleHands[i].fingers.size(); j++){
                int fingerID = simpleHands[i].fingers[j].id%5;
            }
        }
        
        for(int i = 0; i < newPuppet.expressionZones.size(); i++) {
            
            ExpressionZone expressionZone = newPuppet.expressionZones[i];
            vector<LeapFingerController> fingerControllers = expressionZone.leapFingerControllers;
            
            ofVec3f expressionZonePosition = newPuppet.mesh.getVertex(expressionZone.meshIndex);
            
            for(int j = 0; j < fingerControllers.size(); j++) {
                
                LeapFingerController fingerController = fingerControllers[j];
            
                int fingerX = simpleHands[0].fingers[fingerController.fingerID].pos.x;
                int fingerY = -simpleHands[0].fingers[fingerController.fingerID].pos.y;
                
                newPuppet.expressionZones[i].userControlledDisplacement.x = fingerX;
                newPuppet.expressionZones[i].userControlledDisplacement.y = fingerY;
            
                
            }
            
        }
        
    }
    
    leap.markFrameAsOld();
    
}

void ofApp::recieveOsc() {
    
    
    // todo:
    // new method:
    // recieve osc message: add to hashtable with message/value pairing
    
    
    
    
    // this is crazy code.
    // this should really be its own organized little class.
    
    // ... no, just pass in the values to a new function in Puppet.
    
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
        
        float value = m.getArgAsFloat(0);
        
        // loop through all osc namespaces in each control point
        // ->holy moly! fix this code up!
        
        for(int i = 0; i < newPuppet.expressionZones.size(); i++) {
            
            ExpressionZone expressionZone = newPuppet.expressionZones[i];
            vector<OSCNamespace> namespaces = expressionZone.oscNamespaces;
            
            ofVec3f expressionZonePosition = newPuppet.mesh.getVertex(expressionZone.meshIndex);
            
            for(int j = 0; j < namespaces.size(); j++) {
                
                OSCNamespace namesp = namespaces[j];
                
                if(namesp.message == m.getAddress()) {
                    
                    if(namesp.controlType == "x") {
                        newPuppet.expressionZones[i].userControlledDisplacement.y = value;
                    } else if(namesp.controlType == "y") {
                        newPuppet.expressionZones[i].userControlledDisplacement.x = value;
                    }
                
                }
                
            }
            
        }
        
	}
    
}

// lets us drag an image/puppet directory into the window to load it - very convenient

void ofApp::dragEvent(ofDragInfo info) {
    
    switch (state) {
            
        case LOAD_IMAGE:
            
            if(info.files.size() > 0) {
                newPuppet.setImage(info.files.at(0));
                mesher.setImage(newPuppet.image);
                state = IMAGE_SETTINGS;
            }
            
            break;
            
        case IMAGE_SETTINGS:
            break;
            
        case MESH_GENERATED:
            break;
            
        case PUPPET_STAGE:
            
            if(info.files.size() > 0) {
                newPuppet.load(info.files.at(0));
                state = MESH_GENERATED;
            }
            
            break;
            
        default:
            break;
            
    }
    
}

// mouse stuff for selecting vertices/adding skeleton joints/adding osc nodes/etc.

void ofApp::mouseMoved(int x, int y) {
    
    selectedVertexPosition = ofVec3f(0,0,0);
    
    float closestDistance = MAXFLOAT;
    int closestIndex = -1;
    
    ofMesh mesh = newPuppet.puppet.getDeformedMesh();
    
    for(int i = 0; i < mesh.getVertices().size(); i++) {
        ofVec3f v = mesh.getVertex(i);
        float d = v.distance(ofVec3f(x,y,0));
        if(d < closestDistance) {
            closestDistance = d;
            closestIndex = i;
        }
    }
    
    if(closestIndex != -1) {
        selectedVertexPosition = mesh.getVertex(closestIndex);
        selectedVertexIndex = closestIndex;
    }
    
}

void ofApp::mouseDragged(int x, int y, int button) {
    
    
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    if(state == MESH_GENERATED) {
        newPuppet.addExpressionZone(selectedVertexIndex);
    }
    
}

void ofApp::mouseReleased(int x, int y, int button) {
    
    
    
}
