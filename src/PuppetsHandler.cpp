#include "PuppetsHandler.h"

void PuppetsHandler::setup() {
    
    hoveredVertexIndex = -1;
    selectedVertexIndex = -1;
    selectedPuppetIndex = -1;
    
    leapClickAgainTimer = 0;
    
    recordingPuppet = false;
    
    /*
    Puppet p;
    p.load("puppets/demo-killing-ashkeboos");
    puppets.push_back(p);
     */
    
    ofRegisterKeyEvents(this);
    ofRegisterMouseEvents(this);
    
}

void PuppetsHandler::update(LeapDataHandler *leap,
                            ofxOscReceiver *osc,
                            ofxClickDownMenu *cdmenu) {
    
    // update which vertex the leap is pointing to
    if(   enableLeapControls
       && selectedPuppet() != NULL
       && selectedPuppet()->isBeingEdited
       && cdmenu->phase == PHASE_WAIT) {
        
        hoveredVertexIndex = Utils::getClosestIndex(
            selectedPuppet()->meshDeformer.getDeformedMesh(),
            leap->getFingerScreenPosition(leap->pointingFinger+1).x,
            leap->getFingerScreenPosition(leap->pointingFinger+1).y,
            Puppet::MIN_SELECT_VERT_DIST);
        
    }
    
    // leap interface (flick to to add control point mapped to right hand finger 'i')
    if(connectTimer > 0) {
        connectTimer--;
    }
    
    for(int i = leap->puppetFinger; i < leap->puppetFinger+5; i++) {
        
        if(   leap->renderHands
           && leap->fingerFlicked(i)
           && selectedPuppet() != NULL
           && hoveredVertexIndex != -1
           && leapClickAgainTimer == 0){
            
            connectTimer = 60;
            connectedFinger = i;
            connectedEzone = hoveredVertexIndex;
            connectedPuppet = selectedPuppetIndex;
            
            if(selectedPuppet()->getExpressionZone(hoveredVertexIndex) == NULL) {
                selectedPuppet()->addExpressionZone(hoveredVertexIndex);
            }
            selectedPuppet()->getExpressionZone(hoveredVertexIndex)->leapFingerID = i;
            
            selectedPuppet()->getExpressionZone(hoveredVertexIndex)->parentEzone = selectedPuppet()->expressionZones[0].meshIndex;
            
            leapClickAgainTimer = 20;
            
        }
        
    }
    
    // leap interface (flick a puppet with left index finger to toggle edit mode)
    if(leap->renderHands && leap->fingerFlicked(leap->pointingFinger+1) && leapClickAgainTimer == 0) {
        
        int clickedPuppetIndex = getClosestPuppetIndex(
            leap->getFingerScreenPosition(leap->pointingFinger+1).x,
            leap->getFingerScreenPosition(leap->pointingFinger+1).y);
        
        if(clickedPuppetIndex != -1) {
            
            selectedPuppetIndex = clickedPuppetIndex;
            selectedPuppet()->isBeingEdited = true;
            
        }
        if(selectedPuppetIndex != -1 && clickedPuppetIndex == -1) {
            
            selectedPuppetIndex = -1;
            hoveredVertexIndex = -1;
            selectedVertexIndex = -1;
            
        }
        
        leapClickAgainTimer = 20;
        
    }
    if(leapClickAgainTimer > 0) {
        leapClickAgainTimer--;
    }
    
    // update & send new leap and osc data to puppets
    for(int i = 0; i < puppets.size(); i++) {
        
        if(   !controlsPaused) {
            
            puppets[i].recieveLeapData(leap);
            
            while(osc->hasWaitingMessages()) {
                
                // get the next message
                ofxOscMessage m;
                osc->getNextMessage(&m);
                
                puppets[i].recieveOSCMessage(m, m.getArgAsFloat(0));
            }
            
            puppets[i].update();
        }
        
        if(i != selectedPuppetIndex) {
            puppets[i].isBeingEdited = false;
        }
        
    }
    
    
    // update puppet recorder
    puppetRecorder.update();
    if(recordingPuppet && selectedPuppet() != NULL) {
        puppetRecorder.recordPuppetFrame(selectedPuppet());
    }
    
    // update scene recorder
    sceneRecorder.update();
    if(recordingScene) {
        for(int i = 0; i < recordedPuppets.size(); i++) {
            sceneRecorder.addPuppetToFrame(recordedPuppets[i]);
        }
    }
    
    // update puppet recordings
    for(int i = 0; i < recordedPuppets.size(); i++) {
        recordedPuppets[i].update();
    }
    
}

void PuppetsHandler::draw(LeapDataHandler *leap) {
    
    // big red recording button
    if(recordingScene) {
        ofSetColor(255/2+sin(ofGetElapsedTimef()*10)*255/5, 0, 0);
        ofCircle(50, ofGetHeight()-50, 30);
    }
    
    // draw puppet recordings
    
    for(int i = 0; i < recordedPuppets.size(); i++) {
        recordedPuppets[i].draw();
    }
    
    // draw puppets
    
    for(int i = 0; i < puppets.size(); i++) {
        
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        bool isSelected = i == selectedPuppetIndex;
        bool isBeingRecorded = isSelected && recordingPuppet;
        puppets[i].draw(isSelected, isBeingRecorded);
        
    }
    
    // draw currently selected vertex info
    
    if(selectedPuppet() != NULL && selectedVertexIndex != -1) {
        float blinkAlpha = 150+sin(ofGetElapsedTimef()*10)*100;
        ofSetColor(ofColor(255,0,100,blinkAlpha));
        ofCircle(selectedPuppet()->meshDeformer.getDeformedMesh().getVertex(selectedVertexIndex), 8);
    }
    
    ofSetColor(ofColor(0,200,255));
    ofDrawBitmapString(getSelectedVertexInfo(), ofGetWidth()-350, 160);
    
    // highlight the vertex that the mouse is hovered over
    
    if(hoveredVertexIndex != -1) {
        
        ofSetColor(ofColor(255,0,0,100));
        ofCircle(selectedPuppet()->meshDeformer.getDeformedMesh().getVertex(hoveredVertexIndex), 10);
        
        ofSetColor(ofColor(255,0,0,100));
        ofCircle(selectedPuppet()->meshDeformer.getDeformedMesh().getVertex(hoveredVertexIndex), 5);
    }
    
    // draw bones
    
    if(addingBone && hoveredVertexIndex != -1) {
        ofVec3f fromVertex = selectedPuppet()->meshDeformer.getDeformedMesh().getVertex(boneRootVertexIndex);
        ofVec3f toVertex = selectedPuppet()->meshDeformer.getDeformedMesh().getVertex(hoveredVertexIndex);
        
        ofSetColor(255, 255, 0);
        ofSetLineWidth(3);
        ofLine(fromVertex.x, fromVertex.y, toVertex.x, toVertex.y);
        ofSetLineWidth(1);
    }

    
    // (temp) draw connection between finger and ezone
    if(connectTimer > 0) {
        ofSetColor(0,155,255,255*connectTimer/60.0);
        ofPushStyle();
        ofSetLineWidth(5);
        ofLine(leap->getFingerScreenPosition(connectedFinger).x,
               leap->getFingerScreenPosition(connectedFinger).y,
               puppets[connectedPuppet].meshDeformer.getDeformedMesh().getVertex(puppets[connectedPuppet].getExpressionZone(connectedEzone)->meshIndex).x,
               puppets[connectedPuppet].meshDeformer.getDeformedMesh().getVertex(puppets[connectedPuppet].getExpressionZone(connectedEzone)->meshIndex).y);
        ofPopStyle();
    }
    
}

int PuppetsHandler::getClosestPuppetIndex(int x, int y) {
    
    int closestPuppetIndex = -1;
    
    for(int p = 0; p < puppets.size(); p++) {
        if(Utils::isPointInsideMesh(puppets[p].meshDeformer.getDeformedMesh(), x, y)) {
            closestPuppetIndex = p;
            break;
        }
    }
    
    return closestPuppetIndex;
    
}

int PuppetsHandler::getClosestRecordingIndex(int x, int y) {
    
    int closestRecordingIndex = -1;
    
    for(int p = 0; p < recordedPuppets.size(); p++) {
        if(Utils::isPointInsideMesh(recordedPuppets[p].getCurrentMesh(), x, y)) {
            closestRecordingIndex = p;
            break;
        }
    }
    
    return closestRecordingIndex;
    
}

Puppet* PuppetsHandler::selectedPuppet() {
    
    if(selectedPuppetIndex == -1) {
        return NULL;
    } else {
        return &puppets[selectedPuppetIndex];
    }
    
}

string PuppetsHandler::getSelectedVertexInfo() {
    
    string vertInfo = "";
    
    if(selectedPuppet() != NULL && selectedVertexIndex != -1) {
        
        vertInfo += "Mesh index " + ofToString(selectedVertexIndex) + "\n\n";
        
        ExpressionZone* eZone = selectedPuppet()->getExpressionZone(selectedVertexIndex);
        
        if(eZone != NULL) {
            
            if(eZone->oscNamespaces.size() > 0) vertInfo += "OSC namespaces:\n";
            for(int i = 0; i < eZone->oscNamespaces.size(); i++) {
                vertInfo += "    message:     " + eZone->oscNamespaces[i].message     + ":\n";
                vertInfo += "    controlType: " + eZone->oscNamespaces[i].controlType + ":\n";
            }
            
            if(eZone->leapFingerID != -1) {
                vertInfo += "    fingerID: " + ofToString(eZone->leapFingerID) + ":\n";
            }
            
        } else {
            vertInfo += "Selected vertex has no expression zone.";
        }
        
    }
    
    return vertInfo;
    
}

void PuppetsHandler::addPuppet(Puppet p) {
    
    puppets.push_back(p);
    selectedPuppetIndex = puppets.size()-1;
    
}

Puppet *PuppetsHandler::getPuppet(int i) {
    
    if(i < 0 || i >= puppets.size()) {
        return NULL;
    } else {
        return &puppets[i];
    }
    
}

void PuppetsHandler::mousePressed(ofMouseEventArgs &mouse){
    
    if(!cdMenuOpen) {
        
        int button = mouse.button;
        
        if(button == 2) {
            selectedRecordingIndex = getClosestRecordingIndex(mouse.x-ofGetWidth()/2, mouse.y-ofGetHeight()/2);
        }
        
        int clickedPuppetIndex = getClosestPuppetIndex(mouse.x-ofGetWidth()/2, mouse.y-ofGetHeight()/2);
        if(button == 0 && clickedPuppetIndex == selectedPuppetIndex && clickedPuppetIndex != -1) {
            
            // clicked on the same puppet. so edit that puppet.
            //selectedPuppet()->isBeingEdited = !selectedPuppet()->isBeingEdited;
            
        }
            
            if(clickedPuppetIndex != selectedPuppetIndex
                  && hoveredVertexIndex == -1) {
            
            // select a puppet if we clicked on it (or deselect the currently selected puppet)
            
            if(clickedPuppetIndex == -1) {
                
                // we didn't click a puppet. so deselect everything.
                hoveredVertexIndex = -1;
                selectedPuppetIndex = -1;
                selectedVertexIndex = -1;
                
            } else {
                
                // put the newly selected puppet at the back of the list
                // (this makes it so it draws last, putting it in front of the others.)
                Puppet movedPuppet = puppets[clickedPuppetIndex];
                puppets.erase(puppets.begin() + clickedPuppetIndex);
                puppets.push_back(movedPuppet);
                
                // update what's actually selected
                hoveredVertexIndex = -1;
                selectedPuppetIndex = puppets.size()-1;
                selectedVertexIndex = -1;
                
            }
            
        } else {
            
            // selected puppet didn't change, so select/deselect an expression zone
            
            if(addingBone) {
                
                // add bone
                
                if(hoveredVertexIndex != -1) {
                    ExpressionZone* eZone = selectedPuppet()->getExpressionZone(hoveredVertexIndex);
                    if(eZone != NULL) {
                        eZone->parentEzone = boneRootVertexIndex;
                        addingBone = false;
                    }
                }
                
            } else if(hoveredVertexIndex == -1) {
                
                //user clicked away from mesh, so deselect the current vertex.
                selectedVertexIndex = -1;
                
            } else {
                
                ExpressionZone* eZone = selectedPuppet()->getExpressionZone(hoveredVertexIndex);
                
                if(eZone != NULL) {
                    // if there's an expression zone there, select it.
                    selectedVertexIndex = hoveredVertexIndex;
                } else {
                    selectedVertexIndex = -1;
                }
                
            }
            
        }

        
    }
    
}

void PuppetsHandler::mouseDragged(ofMouseEventArgs &mouse){
    
    if(selectedPuppet() != NULL) {
        
        ExpressionZone *ezone = selectedPuppet()->getExpressionZone(selectedVertexIndex);
        
        if(ezone != NULL && ezone->isAnchorPoint) {
            ezone->userControlledDisplacement = ofVec2f(mouse.x-ofGetWidth()/2, mouse.y-ofGetHeight()/2) - selectedPuppet()->mesh.getVertex(selectedVertexIndex);
            selectedPuppet()->update();
        }
        
    }
    
}

void PuppetsHandler::mouseMoved(ofMouseEventArgs &mouse){
    
    if(   !cdMenuOpen
       && selectedPuppet() != NULL
       && selectedPuppet()->isBeingEdited
       && !enableLeapControls) {
        
        hoveredVertexIndex = Utils::getClosestIndex(selectedPuppet()->meshDeformer.getDeformedMesh(),
                                                    mouse.x-ofGetWidth()/2, mouse.y-ofGetHeight()/2, Puppet::MIN_SELECT_VERT_DIST);
    
    }
    
}

void PuppetsHandler::mouseReleased(ofMouseEventArgs &mouse){
    
}

void PuppetsHandler::keyPressed(ofKeyEventArgs &key){
    
    int keyc = key.key;
    
    // delete selected puppet
    if(keyc == OF_KEY_BACKSPACE && selectedPuppet() != NULL) {
        puppets.erase(puppets.begin() + selectedPuppetIndex);
        selectedPuppetIndex = -1;
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
    }
    
    // edit selected puppet
    
    if(keyc == OF_KEY_TAB && selectedPuppet() != NULL) {
        selectedPuppet()->isBeingEdited = !selectedPuppet()->isBeingEdited;
    }
    
    // toggle puppet recording
    if(keyc == 'r') {
        if(!recordingPuppet) {
            puppetRecorder.setup();
            recordingPuppet = true;
        } else {
            recordedPuppets.push_back(puppetRecorder);
            recordingPuppet = false;
        }
    }
    
    // toggle scene recording
    if(keyc == 's') {
        if(!recordingScene) {
            sceneRecorder.setup();
            recordingScene = true;
        } else {
            sceneRecorder.exportAsMovie();
            recordingScene = false;
        }
    }
    
    if(keyc == ' ') {
        controlsPaused = !controlsPaused;
    }
    
}

void PuppetsHandler::keyReleased(ofKeyEventArgs &key){
    
}

void PuppetsHandler::recieveMenuCommand(string command) {
    
    if (command == "menu::add ezone") {
        
        ExpressionZone* eZone = selectedPuppet()->getExpressionZone(hoveredVertexIndex);
        
        if(eZone == NULL) {
            // if there's no expression zone where we clicked, add one.
            selectedPuppet()->addExpressionZone(hoveredVertexIndex);
            selectedVertexIndex = hoveredVertexIndex;
        }
        
    }
    if (command == "menu::add leap mapping::none") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = -1;
    }
    if (command == "menu::add leap mapping::thumb  (hand 1)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 0;
    }
    if (command == "menu::add leap mapping::index  (hand 1)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 1;
    }
    if (command == "menu::add leap mapping::middle (hand 1)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 2;
    }
    if (command == "menu::add leap mapping::ring   (hand 1)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 3;
    }
    if (command == "menu::add leap mapping::pinky  (hand 1)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 4;
    }
    if (command == "menu::add leap mapping::thumb  (hand 2)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 5;
    }
    if (command == "menu::add leap mapping::index  (hand 2)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 6;
    }
    if (command == "menu::add leap mapping::middle (hand 2)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 7;
    }
    if (command == "menu::add leap mapping::ring   (hand 2)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 8;
    }
    if (command == "menu::add leap mapping::pinky  (hand 2)") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 9;
    }
    if (command == "menu::add osc mapping") {
        
        if(selectedPuppet()->getExpressionZone(selectedVertexIndex) != NULL) {
            OSCNamespace namesp;
            namesp.message = ofSystemTextBoxDialog("osc message?");
            namesp.controlType = ofSystemTextBoxDialog("control type?");
            selectedPuppet()->getExpressionZone(selectedVertexIndex)->oscNamespaces.push_back(namesp);
            
        }
        
    }
    if (command == "menu::add bone") {
        
        addingBone = true;
        boneRootVertexIndex = selectedVertexIndex;
        
    }
    if (command == "menu::set anchor point") {
        
        // set the selected ezone to be an anchor point
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->isAnchorPoint = true;
        
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->userControlledDisplacement = selectedPuppet()->meshDeformer.getDeformedMesh().getVertex(selectedVertexIndex) - selectedPuppet()->mesh.getVertex(selectedVertexIndex);
        
    }
    if (command == "menu::remove ezone") {
        
        selectedPuppet()->removeExpressionZone(selectedVertexIndex);
        selectedVertexIndex = -1;
        
    }
    
    if (command == "menu::export puppet") {
        
        ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
        
        if (saveFileResult.bSuccess){
            string path = saveFileResult.getPath();
            selectedPuppet()->save(path);
        }
        
    }
    if (command == "menu::edit puppet") {
        
        selectedPuppet()->isBeingEdited = true;
        
    }
    if (command == "menu::remove puppet") {
        
        puppets.erase(puppets.begin() + selectedPuppetIndex);
        
        selectedPuppetIndex = -1;
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    if (command == "menu::reset puppet") {
        
        selectedPuppet()->reset();
        
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    if (command == "menu::export recording as mov") {
        
        recordedPuppets[selectedRecordingIndex].exportAsMovie();
        
    }
    if (command == "menu::remove recording") {
        
        recordedPuppets.erase(recordedPuppets.begin() + selectedRecordingIndex);
        
    }
    if (command == "menu::clear all puppets") {
        
        puppets.clear();
        
        selectedPuppetIndex = -1;
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    if (command == "menu::clear all recordings") {
        
        recordedPuppets.clear();
        
        selectedPuppetIndex = -1;
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }

    
}