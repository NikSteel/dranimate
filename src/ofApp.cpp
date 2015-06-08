#include "ofApp.h"

void ofApp::setup() {
    
    receiver.setup(8000);
    leap.open();
    
    mesher.setup();
    
    leapFingersPositions.resize(5);
    leapFingersCalibration.resize(5);
    leapCalibrated = false;
    
    state = PUPPET_STAGE;
    transformState = NONE;
    
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
            
            if(transformState == SCALE) {
                newPuppet.scaleMesh(scaleFromPoint, ofVec2f(mouseX,mouseY));
            }
            if(transformState == ROTATE) {
                newPuppet.rotateMesh(scaleFromPoint, ofVec2f(mouseX,mouseY));
            }
            
            selectClosestVertex();
            
            recieveOsc();
            recieveLeap();
            
            newPuppet.update();
            
            break;
            
        case LEAP_CALIBRATION:
            recieveLeap();
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
            ofDrawBitmapString("l   -   Load an image to make a puppet with", ofGetWidth()-400, 30);
            
            break;
    
        case IMAGE_SETTINGS:
        
            mesher.draw();
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("m   -   Generate mesh when ready", ofGetWidth()-400, 30);
            
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
                vertInfo += "o  -   Add osc mapping\n";
                vertInfo += "l  -   Add leap mapping\n\n";
                
                ExpressionZone* eZone = newPuppet.getExpressionZone(selectedVertexIndex);
                
                if(eZone != NULL) {
                    
                    if(eZone->oscNamespaces.size() > 0) vertInfo += "OSC namespaces:\n";
                    for(int i = 0; i < eZone->oscNamespaces.size(); i++) {
                        vertInfo += "    message:     " + eZone->oscNamespaces[i].message     + ":\n";
                        vertInfo += "    controlType: " + eZone->oscNamespaces[i].controlType + ":\n";
                    }
                    
                    if(eZone->leapFingerControllers.size() > 0) vertInfo += "\nLeap controller mappings:\n";
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
            ofDrawBitmapString(vertInfo, ofGetWidth()-350, 100);
            
            if(!leapCalibrated) {
                ofSetColor(255,0,0);
                ofDrawBitmapString("No leap calibration!", ofGetWidth()-350, 30);
            }
            
            // instructions
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("e    -   Export current puppet\nw    -   Toggle rendering wireframe\nc    -    Calibrate leap contoller", ofGetWidth()-350, 60);
            
            break;
            
        }
            
        case LEAP_CALIBRATION:
            
            for(int i = 0; i < 5; i++) {
                ofSetColor(255,255,0);
                int x = leapFingersPositions[i].x;
                int y = -leapFingersPositions[i].y;
                ofCircle(x, y, 5);
                ofDrawBitmapString(ofToString(i), x+5, y+5);
            }
            ofCircle(palmPosition.x, -palmPosition.y, 5);
            ofDrawBitmapString("Palm", palmPosition.x+5, -palmPosition.y+5);
            
            if(leapCalibrated) {
                for(int i = 0; i < 5; i++) {
                    ofSetColor(0,255,255);
                    int x = leapFingersCalibration[i].x;
                    int y = -leapFingersCalibration[i].y;
                    ofCircle(x, y, 5);
                    ofDrawBitmapString(ofToString(i), x+5, y+5);
                }
                ofCircle(calibratedPalmPosition.x, -calibratedPalmPosition.y, 5);
                ofDrawBitmapString("Palm", calibratedPalmPosition.x+5, -calibratedPalmPosition.y+5);
            }
            
            if(!leapCalibrated) {
                ofSetColor(255,0,0);
                ofDrawBitmapString("No leap calibration!", ofGetWidth()-400, 30);
                
                ofSetColor(255,255,255);
                ofDrawBitmapString("Place hand above the leap controller and position fingers in a resting position.", ofGetWidth()/2-300, ofGetHeight()-100);
            }
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("c   -   Set calibration\nl   -   Load existing calibration\ns   -   Save current calibration\n\nd    -    Go back", ofGetWidth()-400, 60);
            
            break;
            
        case PUPPET_STAGE:
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("l   -   Load a puppet\ns   -   Create a new puppet", ofGetWidth()-400, 30);
            
            // todo: draw all of the currently loaded puppets.
            
            break;
            
    }
    
}

void ofApp::keyReleased(int key) {
    
    switch (state) {
            
        case LOAD_IMAGE:
            
            // load image to use for new puppet.
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
            
            // generate mesh
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
                    if(newPuppet.getExpressionZone(selectedVertexIndex)->leapFingerControllers.size() == 0) {
                        LeapFingerController fingerController;
                        fingerController.fingerID = 0;
                        newPuppet.addFingerControllerToExpressionZone(selectedVertexIndex, fingerController);
                    } else {
                        int i = newPuppet.getExpressionZone(selectedVertexIndex)->leapFingerControllers[0].fingerID;
                        newPuppet.getExpressionZone(selectedVertexIndex)->leapFingerControllers.pop_back();
                        LeapFingerController fingerController;
                        fingerController.fingerID = i+1;
                        newPuppet.addFingerControllerToExpressionZone(selectedVertexIndex, fingerController);
                    }
                }
                
            }
            
            // scale from a point
            if(key == 's') {
                scaleFromPoint = ofVec2f(mouseX,mouseY);
                newPuppet.beginScale();
                transformState = SCALE;
            }
            
            // rotate around a point
            if(key == 'r') {
                scaleFromPoint = ofVec2f(mouseX,mouseY);
                newPuppet.beginRotate();
                transformState = ROTATE;
            }
            
            // switch to leap calibration mode
            if(key == 'c') {
                state = LEAP_CALIBRATION;
            }
        
            break;
            
        case LEAP_CALIBRATION:
            
            // load leap calibration
            if(key == 'l') {
                //todo:load calibration
            }
            
            // set leap calibration
            if(key == 'c') {
                for(int i = 0; i < 5; i++) {
                    leapFingersCalibration[i] = leapFingersPositions[i];
                }
                calibratedPalmPosition = palmPosition;
                leapCalibrated = true;
            }
            
            if(key == 'd') {
                state = MESH_GENERATED;
            }
            
            break;
            
        case PUPPET_STAGE:
            
            // load puppet
            if(key == 'l') {
                
                ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a puppet directory:",true);
                
                if (openFileResult.bSuccess){
                    newPuppet.load(openFileResult.getPath());
                    state = MESH_GENERATED;
                }
                
            }
            
            // for debugging. replace with something else soon
            if(key == 's') {
                state = LOAD_IMAGE;
            }
            
            break;
            
        default:
            break;
            
    }
    
    // temporary back-to-beginning key (reset everything)
    // replace with something else soon
    if(key == 'r') {
        state = PUPPET_STAGE;
    }
    
    if(key == 'f') {
        ofToggleFullscreen();
    }
    
}

void ofApp::recieveLeap() {
    
    // this is crazy
    // please make a class for this
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-460, 100, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-300, 0, -200, 200);
        
        for(int i = 0; i < newPuppet.expressionZones.size(); i++) {
            
            ExpressionZone expressionZone = newPuppet.expressionZones[i];
            vector<LeapFingerController> fingerControllers = expressionZone.leapFingerControllers;
            
            ofVec3f expressionZonePosition = newPuppet.mesh.getVertex(expressionZone.meshIndex);
            
            int handX = simpleHands[0].handPos.x;
            int handY = -simpleHands[0].handPos.y;
            
            //newPuppet.expressionZones[i].userControlledDisplacement.x = handX;
            //newPuppet.expressionZones[i].userControlledDisplacement.y = handY;
            
            palmPosition = ofVec3f(handX,handY,0);
            for(int h = 0; h < 5; h++) {
                leapFingersPositions[h] = ofVec3f(simpleHands[0].fingers[h].pos.x,
                                                  simpleHands[0].fingers[h].pos.y,
                                                  simpleHands[0].fingers[h].pos.z);
            }
            
            for(int j = 0; j < fingerControllers.size(); j++) {
                
                LeapFingerController fingerController = fingerControllers[j];
            
                int fingerX = simpleHands[0].fingers[fingerController.fingerID].pos.x;
                int fingerY = -simpleHands[0].fingers[fingerController.fingerID].pos.y;
                
                newPuppet.expressionZones[i].userControlledDisplacement.x = ofGetWidth()/3+fingerX-leapFingersCalibration[fingerController.fingerID].x;
                newPuppet.expressionZones[i].userControlledDisplacement.y = ofGetHeight()/3+fingerY+leapFingersCalibration[fingerController.fingerID].y;
                
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
    
}

void ofApp::mouseDragged(int x, int y, int button) {
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    switch(state) {
            
        case MESH_GENERATED:
            if(selectedVertexIndex != -1 && transformState == NONE) {
                
                ExpressionZone* eZone = newPuppet.getExpressionZone(selectedVertexIndex);
                
                if(eZone == NULL) {
                    newPuppet.addExpressionZone(selectedVertexIndex);
                }
                
            }
            
            if(transformState != NONE) {
                transformState = NONE;
            }
            
            break;
            
        default:
            break;
            
    }
    
}

void ofApp::mouseReleased(int x, int y, int button) {
    
    
    
}
