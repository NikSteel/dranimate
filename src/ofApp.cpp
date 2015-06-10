#include "ofApp.h"

void ofApp::setup() {
    
    receiver.setup(8000);
    leap.open();
    
    mesher.setup();
    
    recievingLeap = true;
    leapFingersPositions.resize(5);
    leapFingersCalibration.resize(5);
    leapCalibrated = false;
    
    state = PUPPET_STAGE;
    transformState = NONE;
    
    // load a demo puppet
    newPuppet.load("exported-puppets/demo-killingashkeboos/");
    state = MESH_GENERATED;
    
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
            if(recievingLeap) recieveLeap();
            
            newPuppet.update();
            puppetRecorder.update();
            
            if(recordingPuppet) {
                puppetRecorder.recordPuppetFrame(newPuppet);
            }
            
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
            
            newPuppet.draw(drawWireframe, transformState != NONE);
            
            // draw puppet recordings
            
            puppetRecorder.draw();
            
            // draw currently selected vertex info
            
            string vertInfo = "";
            vertInfo += "Selected vertex info:\n";
            
            if(selectedVertexIndex != -1) {
                
                ofSetColor(ofColor(255,0,255));
                ofCircle(newPuppet.puppet.getDeformedMesh().getVertex(selectedVertexIndex), 10);
                ofSetColor(ofColor(255,255,0));
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
                    
                    if(eZone->leapFingerID != -1) {
                        vertInfo += "    fingerID: " + ofToString(eZone->leapFingerID) + ":\n";
                    }
                    
                    if(eZone->isRoot) {
                         vertInfo += "    This control point is the root. ";
                    }
                    
                } else {
                    vertInfo += "Selected vertex has no expression zone.";
                }
                
            } else {
                vertInfo += "No selected vertex.";
            }
            
            ofSetColor(ofColor(0,200,255));
            ofDrawBitmapString(vertInfo, ofGetWidth()-350, 100);
            
            // highlight the vertex that the mouse is hovered over
            
            if(hoveredVertexIndex != -1) {
                ofSetColor(ofColor(0,155,255));
                ofCircle(newPuppet.puppet.getDeformedMesh().getVertex(hoveredVertexIndex), 5);
            }
            
            if(!leapCalibrated) {
                ofSetColor(255,0,0);
                ofDrawBitmapString("No leap calibration!", ofGetWidth()-350, 30);
            }
            
            // instructions
            
            ofSetColor(255,255,255);
            ofDrawBitmapString("e    -   Export current puppet\nw    -   Toggle rendering wireframe\nn    -    Calibrate leap contoller", ofGetWidth()-350, 60);
            
            break;
            
        }
            
        case LEAP_CALIBRATION:
            
            for(int i = 0; i < 5; i++) {
                ofSetColor(255,255,0);
                int x = leapFingersPositions[i].x;
                int y = leapFingersPositions[i].y;
                ofCircle(x, y, 5);
                ofDrawBitmapString(ofToString(i), x+5, y+5);
            }
            ofCircle(palmPosition.x, palmPosition.y, 5);
            ofDrawBitmapString("Palm", palmPosition.x+5, palmPosition.y+5);
            
            if(leapCalibrated) {
                for(int i = 0; i < 5; i++) {
                    ofSetColor(0,255,255);
                    int x = leapFingersCalibration[i].x;
                    int y = leapFingersCalibration[i].y;
                    ofCircle(x, y, 5);
                    ofDrawBitmapString(ofToString(i), x+5, y+5);
                }
                ofCircle(calibratedPalmPosition.x, calibratedPalmPosition.y, 5);
                ofDrawBitmapString("Palm", calibratedPalmPosition.x+5, calibratedPalmPosition.y+5);
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
                    newPuppet.reset();
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
                    newPuppet.getExpressionZone(selectedVertexIndex)->oscNamespaces.push_back(namesp);

                }
                
            }
            
            // add leap controller mapping
            if(key == 'l') {
                if(newPuppet.getExpressionZone(selectedVertexIndex) != NULL) {
                    ExpressionZone *eZone = newPuppet.getExpressionZone(selectedVertexIndex);
                    if(eZone->leapFingerID == 4) {
                        eZone->leapFingerID = -1;
                    } else {
                        eZone->leapFingerID++;
                    }
                }
            }
            
            // set vertex to root
            if(key == 'r') {
                if(newPuppet.getExpressionZone(selectedVertexIndex) != NULL) {
                    newPuppet.makeExpressionZoneRoot(selectedVertexIndex);
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
                // todo: rotate
                newPuppet.beginRotate();
                transformState = ROTATE;
            }
            
            // switch to leap calibration mode
            if(key == 'c') {
                state = LEAP_CALIBRATION;
            }
            
            // toggle puppet leap control
            if(key == ' ') {
                recievingLeap = !recievingLeap;
            }
            
            // toggle puppet recording
            if(key == 'p') {
                if(!recordingPuppet) {
                    puppetRecorder.setup();
                    recordingPuppet = true;
                } else {
                    recordingPuppet = false;
                }
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
            
            // done, go back
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
            
            if(key == 's') {
                state = LOAD_IMAGE;
            }
            
            break;
            
        default:
            break;
            
    }
    
    // back to menu
    if(key == 'b') {
        state = PUPPET_STAGE;
    }
    
    // toggle fullscreen
    if(key == 'f') {
        ofToggleFullscreen();
    }
    
}

void ofApp::recieveLeap() {
    
    simpleHands = leap.getSimpleHands();
    
    if( leap.isFrameNew() && simpleHands.size() ){
        
        leap.setMappingX(-460, 100, -ofGetWidth()/2, ofGetWidth()/2);
		leap.setMappingY(90, 490, -ofGetHeight()/2, ofGetHeight()/2);
        leap.setMappingZ(-300, 0, -200, 200);
        
        palmPosition = ofVec3f(simpleHands[0].handPos.x,
                               -simpleHands[0].handPos.y,
                               0);
        
        for(int i = 0; i < 5; i++) {
            leapFingersPositions[i] = ofVec3f(simpleHands[0].fingers[i].pos.x,
                                              -simpleHands[0].fingers[i].pos.y,
                                              simpleHands[0].fingers[i].pos.z);
        }
        
        newPuppet.recieveLeapData(leapFingersPositions,
                                  leapFingersCalibration,
                                  palmPosition,
                                  calibratedPalmPosition);
        
    }
    
    leap.markFrameAsOld();
    
}

void ofApp::recieveOsc() {
    
    float gyroX = 0;
    float gyroY = 0;
    
    bool gotX = false;
    bool gotY = false;
    
    // check for waiting messages
	while(receiver.hasWaitingMessages()) {
        
		// get the next message
		ofxOscMessage message;
		receiver.getNextMessage(&message);
        
        float value = message.getArgAsFloat(0);
        
        newPuppet.recieveOSCMessage(message, value);
        
	}
    
}

// lets us drag an image/puppet directory into the window to load it - very convenient

void ofApp::dragEvent(ofDragInfo info) {
    
    switch (state) {
            
        case LOAD_IMAGE:
            
            if(info.files.size() > 0) {
                newPuppet.reset();
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
                newPuppet.reset();
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
    
    hoveredVertexIndex = closestIndex;
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    switch(state) {
            
        case MESH_GENERATED:
            if(transformState == NONE) {
                
                if(hoveredVertexIndex == -1) {
                    
                    //user clicked away from mesh, so deselect the current vertex.
                    selectedVertexIndex = -1;
                    
                } else {
                    
                    ExpressionZone* eZone = newPuppet.getExpressionZone(hoveredVertexIndex);
                    
                    if(eZone == NULL) {
                        // if there's no expression zone where we clicked, add one.
                        newPuppet.addExpressionZone(hoveredVertexIndex);
                        selectedVertexIndex = hoveredVertexIndex;
                    } else {
                        // if there IS an expression zone there, select it.
                        selectedVertexIndex = hoveredVertexIndex;
                    }
                    
                }
                
            }
            
            // user clicked to finish a scale/rotation.
            if(transformState != NONE) {
                transformState = NONE;
            }
            
            break;
            
        default:
            break;
            
    }
    
}
