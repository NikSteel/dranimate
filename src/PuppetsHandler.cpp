#include "PuppetsHandler.h"

// public methods

void PuppetsHandler::setup() {
    
    hoveredVertexIndex = -1;
    selectedVertexIndex = -1;
    selectedPuppetIndex = -1;
    
    leapClickAgainTimer = 0;
    
    Puppet p;
    p.load("puppets/demo-killing-ashkeboos");
    puppets.push_back(p);
    
    recording = false;
    
}
void PuppetsHandler::update(LeapDataHandler *leap,
                            ofxOscReceiver *osc,
                            ofxClickDownMenu *cdmenu) {
    
    // check which type of ui control we're using
    //(if the leap found two hands, control ui with leap)
    if(leap->getHandCount() == 2) {
        currentUIControlType = LEAP;
    } else {
        currentUIControlType = MOUSE;
    }
    
    if(currentUIControlType == LEAP) {
        updateLeapUIControls(leap,cdmenu);
    }
    
    for(int i = 0; i < puppets.size(); i++) {
        
        if(puppets[i].isControllable()) {
            // send new leap data to puppet
            puppets[i].recieveLeapData(leap, i == selectedPuppetIndex);
            
            // send new osc messages to puppet
            while(osc->hasWaitingMessages()) {
                ofxOscMessage m;
                osc->getNextMessage(&m);
                puppets[i].recieveOSCMessage(m, m.getArgAsFloat(0));
            }
        }
        
        // update puppet
        puppets[i].update();
        
    }
    
    // record puppets
    if(recording && puppets.size() > 0) {
        
        newRecording.addFrame(puppets[0].getDeformedMesh(), puppets[0].getPosition(), puppets[0].getRotation());
        
    }
    
}
void PuppetsHandler::draw(LeapDataHandler *leap) {
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    // draw puppets
    for(int i = 0; i < puppets.size(); i++) {
        
        glEnable(GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        bool isSelected = i == selectedPuppetIndex;
        puppets[i].draw(isSelected,recording);
        
    }
    
    // draw currently selected vertex info
    if(selectedPuppet() != NULL && selectedVertexIndex != -1) {
        float blinkAlpha = 150+sin(ofGetElapsedTimef()*10)*100;
        ofSetColor(ofColor(255,0,100,blinkAlpha));
        ofCircle(selectedPuppet()->getDeformedMesh().getVertex(selectedVertexIndex), 8);
    }
    
    ofSetColor(ofColor(0,200,255));
    ofDrawBitmapString(getSelectedVertexInfo(), ofGetWidth()-350, 160);
    
    // highlight the vertex that the mouse is hovered over
    if(hoveredVertexIndex != -1) {
        
        ofSetColor(ofColor(255,0,0,100));
        ofCircle(selectedPuppet()->getDeformedMesh().getVertex(hoveredVertexIndex), 10);
        
        ofSetColor(ofColor(255,0,0,100));
        ofCircle(selectedPuppet()->getDeformedMesh().getVertex(hoveredVertexIndex), 5);
    }
    
    // draw bones
    if(addingBone && hoveredVertexIndex != -1) {
        ofVec3f fromVertex = selectedPuppet()->getDeformedMesh().getVertex(boneRootVertexIndex);
        ofVec3f toVertex = selectedPuppet()->getDeformedMesh().getVertex(hoveredVertexIndex);
        
        ofSetColor(255, 255, 0);
        ofSetLineWidth(3);
        ofLine(fromVertex.x, fromVertex.y, toVertex.x, toVertex.y);
        ofSetLineWidth(1);
    }
    
    ofPopMatrix();
    
}

void PuppetsHandler::addPuppet(Puppet p) {
    
    puppets.push_back(p);
    selectedPuppetIndex = puppets.size()-1;
    
}

void PuppetsHandler::loadPuppet(string path) {
    
    Puppet loadedPuppet;
    loadedPuppet.load(path);
    addPuppet(loadedPuppet);
    
}
void PuppetsHandler::loadRecording(string path) {
    
    Puppet loadedPuppet;
    loadedPuppet.loadCachedFrames(path);
    addPuppet(loadedPuppet);
    
}

void PuppetsHandler::loadScene() {
    
    puppets.clear();
    
    ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a scene directory:",true);
    
    if (openFileResult.bSuccess){
        
        string path = openFileResult.getPath();
        
        ofxXmlSettings info;
        if(info.loadFile(path + "/info.xml")) {
        
            int nPuppets = info.getNumTags("puppet");
            for(int i = 0; i < nPuppets; i++) {
                
                info.pushTag("puppet", i);
                
                bool isControllable = info.getValue("isControllable", -1);
                string puppetDirPath = info.getValue("puppetdir", "");
                
                if(isControllable) {
                    loadPuppet(path+"/"+puppetDirPath);
                } else {
                    loadRecording(path+"/"+puppetDirPath);
                }
                
                info.popTag();
                
            }
        
        }
    }
    
}
void PuppetsHandler::exportScene() {
    
    ofFileDialogResult saveFileResult = ofSystemSaveDialog("newscene", "Select location to export scene:");
    
    if (saveFileResult.bSuccess){
       
        string path = saveFileResult.getPath();
        
        string mkdirCommandString = "mkdir " + path;
        system(mkdirCommandString.c_str());
        
        ofxXmlSettings info;
        for(int i = 0; i < puppets.size(); i++) {
            
            info.addTag("puppet");
            info.pushTag("puppet",i);
            info.addValue("puppetdir", "puppet"+ofToString(i));
            info.addValue("isControllable", puppets[i].isControllable());
            info.popTag();
            
            if(puppets[i].isControllable()) {
                puppets[i].save(path + "/puppet" + ofToString(i));
            } else {
                puppets[i].saveCachedFrames(path + "/puppet" + ofToString(i));
            }
            
        }
        info.save(path + "/info.xml");
        
    }
    
}

Puppet* PuppetsHandler::selectedPuppet() {
    
    if(selectedPuppetIndex == -1) {
        return NULL;
    } else {
        return &puppets[selectedPuppetIndex];
    }
    
}
bool PuppetsHandler::isAPuppetSelected() {
    
    return selectedPuppet() != NULL;
    
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

void PuppetsHandler::clickMouseAt(int x, int y) {
    
    int clickedPuppetIndex = getClosestPuppetIndex(x-ofGetWidth() /2,
                                                   y-ofGetHeight()/2);
    
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

void PuppetsHandler::updateWhichVertexIsHoveredOver(int x, int y) {
    
    if(   selectedPuppet() != NULL
       && selectedPuppet()->isControllable()
       && !enableLeapControls) {
        
        hoveredVertexIndex = Utils::getClosestIndex(
            selectedPuppet()->getDeformedMesh(),
            x-ofGetWidth()/2  + selectedPuppet()->getPosition().x,
            y-ofGetHeight()/2 + selectedPuppet()->getPosition().y,
            Puppet::MIN_SELECT_VERT_DIST
        );
        
    }
    
}
bool PuppetsHandler::emptyVertexHoveredOver() {
    
    return hoveredVertexIndex != -1
    && selectedVertexIndex != hoveredVertexIndex;
    
}
bool PuppetsHandler::ezoneHoveredOver() {
    
    return selectedVertexIndex != -1
    && selectedVertexIndex == hoveredVertexIndex;
    
}


void PuppetsHandler::addExpressionZoneToCurrentPuppet() {
    
    ExpressionZone* eZone = selectedPuppet()->getExpressionZone(hoveredVertexIndex);
    
    if(eZone == NULL) {
        // if there's no expression zone where we clicked, add one.
        selectedPuppet()->addExpressionZone(hoveredVertexIndex);
        selectedVertexIndex = hoveredVertexIndex;
    }
    
}
void PuppetsHandler::addLeapMappingToCurrentPuppet(int i) {
    
    selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = i;
    
}
void PuppetsHandler::addOSCMappingToCurrentPuppet() {
    
    if(selectedPuppet()->getExpressionZone(selectedVertexIndex) != NULL) {
        OSCNamespace namesp;
        namesp.message = ofSystemTextBoxDialog("osc message?");
        namesp.controlType = ofSystemTextBoxDialog("control type?");
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->oscNamespaces.push_back(namesp);
        
    }
    
}
void PuppetsHandler::addBoneToCurrentPuppet() {
    
    addingBone = true;
    boneRootVertexIndex = selectedVertexIndex;
    
}
void PuppetsHandler::removeEZoneFromCurrentPuppet() {
    
    selectedPuppet()->removeExpressionZone(selectedVertexIndex);
    selectedVertexIndex = -1;
    
}
void PuppetsHandler::setAnchorPointOnCurrentPuppet() {
    
    // set the selected ezone to be an anchor point
    selectedPuppet()->getExpressionZone(selectedVertexIndex)->isAnchorPoint = true;
    
    selectedPuppet()->getExpressionZone(selectedVertexIndex)->userControlledDisplacement = selectedPuppet()->getDeformedMesh().getVertex(selectedVertexIndex) - selectedPuppet()->getMesh().getVertex(selectedVertexIndex);
    
}
void PuppetsHandler::exportCurrentPuppet() {
    
    ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
    
    if (saveFileResult.bSuccess){
        string path = saveFileResult.getPath();
        
        if(selectedPuppet()->isControllable()) {
            selectedPuppet()->save(path);
        } else {
            selectedPuppet()->saveCachedFrames(path);
        }
    }
    
}
void PuppetsHandler::removeCurrentPuppet() {
    
    puppets.erase(puppets.begin() + selectedPuppetIndex);
    
    selectedPuppetIndex = -1;
    selectedVertexIndex = -1;
    hoveredVertexIndex = -1;
    
}
void PuppetsHandler::resetCurrentPuppet() {
    
    selectedVertexIndex = -1;
    hoveredVertexIndex = -1;
    
}

void PuppetsHandler::clearAllPupets() {
    
    puppets.clear();
    
    selectedPuppetIndex = -1;
    selectedVertexIndex = -1;
    hoveredVertexIndex = -1;
    
}
void PuppetsHandler::removeAllPuppets() {
    
    puppets.clear();
    
    selectedPuppetIndex = -1;
    selectedVertexIndex = -1;
    hoveredVertexIndex = -1;
    
}

void PuppetsHandler::togglePuppetRecording() {
    
    if(!recording && puppets.size() > 0) {
        
        recording = true;
        
        newRecording.setImage(puppets[0].getImage());
        newRecording.clearCachedFrames();
        newRecording.makeRecording();
        
    } else {
        
        puppets.push_back(newRecording);
        recording = false;
        
    }
    
}

// private methods

Puppet *PuppetsHandler::getPuppet(int i) {
    
    if(i < 0 || i >= puppets.size()) {
        return NULL;
    } else {
        return &puppets[i];
    }
    
}
int PuppetsHandler::getClosestPuppetIndex(int x, int y) {
    
    int closestPuppetIndex = -1;
    
    for(int p = 0; p < puppets.size(); p++) {
        
        if(puppets[p].isPointInside(x,y)) {
            closestPuppetIndex = p;
            break;
        }
        
    }
    
    return closestPuppetIndex;
    
}

void PuppetsHandler::updateLeapUIControls(LeapDataHandler *leap,
                                          ofxClickDownMenu *cdmenu) {
    
    // update which vertex the leap is pointing to
    if(   selectedPuppet() != NULL
       && cdmenu->phase == PHASE_WAIT) {
        
        hoveredVertexIndex = Utils::getClosestIndex(
                                                    selectedPuppet()->getDeformedMesh(),
                                                    leap->getFingerScreenPosition(leap->pointingFinger+1).x,
                                                    leap->getFingerScreenPosition(leap->pointingFinger+1).y,
                                                    Puppet::MIN_SELECT_VERT_DIST);
        
    }
    
    for(int i = leap->puppetFinger; i < leap->puppetFinger+5; i++) {
        
        if(   leap->fingerFlicked(i)
           && selectedPuppet() != NULL
           && hoveredVertexIndex != -1
           && leapClickAgainTimer == 0){
            
            if(selectedPuppet()->getExpressionZone(hoveredVertexIndex) == NULL) {
                selectedPuppet()->addExpressionZone(hoveredVertexIndex);
            }
            selectedPuppet()->getExpressionZone(hoveredVertexIndex)->leapFingerID = i;
            
            // autobone (disabled for now...)
            //selectedPuppet()->getExpressionZone(hoveredVertexIndex)->parentEzone = selectedPuppet()->expressionZones[0].meshIndex;
            
            leapClickAgainTimer = 20;
            
        }
        
    }
    
    // leap interface (flick a puppet with left index finger to toggle edit mode)
    if(leap->fingerFlicked(leap->pointingFinger+1) && leapClickAgainTimer == 0) {
        
        int clickedPuppetIndex = getClosestPuppetIndex(
                                                       leap->getFingerScreenPosition(leap->pointingFinger+1).x,
                                                       leap->getFingerScreenPosition(leap->pointingFinger+1).y);
        
        if(clickedPuppetIndex != -1) {
            
            selectedPuppetIndex = clickedPuppetIndex;
            
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
    
}