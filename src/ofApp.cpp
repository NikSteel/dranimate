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
            
            selectClosestVertex();
            
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
            ofDrawBitmapString("Create a puppet:\nDrag image file into window or press 'l' to load an image", 300, 30);
            
            break;
    
        case IMAGE_SETTINGS:
        
            mesher.draw();
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Adjust image settings and press 'm' to generate mesh when ready", 300, 30);
            
            break;
        
        case MESH_GENERATED: {
            
            // draw puppets
            
            newPuppet.draw(drawWireframe);
            
            // draw currently selected vertex info
            
            string vertInfo = "";
            vertInfo += "Selected vertex info:\n";
            
            if(selectedVertexIndex != -1) {
                
                ofSetColor(ofColor(0,155,255));
                ofCircle(newPuppet.puppet.getDeformedMesh().getVertex(selectedVertexIndex), 5);
                
                vertInfo += "Mesh index " + ofToString(selectedVertexIndex) + "\n\n";
                vertInfo += "Press 'o' to add osc mapping\n";
                vertInfo += "Press 'l' to add leap mapping\n\n";
                
                ExpressionZone* eZone = newPuppet.getExpressionZone(selectedVertexIndex);
                
                if(eZone != NULL) {
                    
                    vertInfo += "OSC namespaces:\n";
                    for(int i = 0; i < eZone->oscNamespaces.size(); i++) {
                        vertInfo += "    message:     " + eZone->oscNamespaces[i].message     + ":\n";
                        vertInfo += "    controlType: " + eZone->oscNamespaces[i].controlType + ":\n";
                    }
                    vertInfo += "\nLeap controller mappings:\n";
                    for(int i = 0; i < eZone->leapFingerControllers.size(); i++) {
                        vertInfo += "    fingerID: " + ofToString(eZone->leapFingerControllers[i].fingerID) + ":\n";
                    }
                } else {
                    vertInfo += "Selected vertex has no expression zone.";
                }
                
            } else {
                vertInfo += "No selected vertex.";
            }
            
            ofSetColor(ofColor(0,200,255));
            ofDrawBitmapString(vertInfo, 500, 100);
            
            // instructions
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'e' to export current puppet\nPress 'w' to toggle rendering wireframe", 300, 30);
            
            break;
            
        }
            
        case PUPPET_STAGE:
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("Press 'l' to load a puppet or drag puppet folder into window\nOr, press 's' to create a new puppet", 300, 30);
            
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
            
            // export current puppet
            if(key == 'e') {
                
                ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
                
                if (saveFileResult.bSuccess){
                    string path = saveFileResult.getPath();
                    newPuppet.save(path);
                }
                
            }
            
            // toggle wireframe rendering
            if(key == 'w') {
                drawWireframe = !drawWireframe;
            }
            
            // add osc mapping
            if(key == 'o') {
                
                if(newPuppet.getExpressionZone(selectedVertexIndex) != NULL) {
                    OSCNamespace namesp;
                    namesp.message = ofSystemTextBoxDialog("osc message?");
                    namesp.controlType = ofSystemTextBoxDialog("control type?");
                    newPuppet.addNamespaceToExpressionZone(selectedVertexIndex, namesp);
                }
                
            }
            
            // add leap controller mapping
            if(key == 'l') {
                
                if(newPuppet.getExpressionZone(selectedVertexIndex) != NULL) {
                    LeapFingerController fingerController;
                    fingerController.fingerID = ofToInt(ofSystemTextBoxDialog("finger ID?"));
                    newPuppet.addFingerControllerToExpressionZone(selectedVertexIndex, fingerController);
                }
                
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

void ofApp::selectClosestVertex() {
    
    // find vertex closest to cursor
    
    float closestDistance = MAXFLOAT;
    int closestIndex = -1;
    
    ofMesh mesh = newPuppet.puppet.getDeformedMesh();
    
    for(int i = 0; i < mesh.getVertices().size(); i++) {
        ofVec3f v = mesh.getVertex(i);
        float d = v.distance(ofVec3f(mouseX,mouseY,0));
        if(d < closestDistance && d < MIN_SELECT_VERT_DIST) {
            closestDistance = d;
            closestIndex = i;
        }
    }
    
    selectedVertexIndex = closestIndex;
    
}

void ofApp::mouseMoved(int x, int y) {
    
    mouseX = x;
    mouseY = y;
    
}

void ofApp::mouseDragged(int x, int y, int button) {
    
    
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    if(state == MESH_GENERATED && selectedVertexIndex != -1) {
        
        ExpressionZone* eZone = newPuppet.getExpressionZone(selectedVertexIndex);
        
        if(eZone == NULL) {
            newPuppet.addExpressionZone(selectedVertexIndex);
        }
        
    }
    
}

void ofApp::mouseReleased(int x, int y, int button) {
    
    
    
}
