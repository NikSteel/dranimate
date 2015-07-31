#include "PuppetsHandler.h"

// public methods

void PuppetsHandler::setup() {
    
    hoveredVertexIndex = -1;
    selectedVertexIndex = -1;
    selectedPuppetIndex = -1;
    
    activeLayer = 1;
    
    leapClickAgainTimer = 0;
    
    if(LOAD_DEMO_PUPPET) {
        Puppet p;
        p.load(DEMO_PUPPET_PATH);
        addPuppet(p);
    }
    
    recording = false;
    
    ofxXmlSettings settings; settings.load(LAUNCH_SETTINGS_PATH);
    numLayers = settings.getValue("numLayers", 0);
    syphonServersEnabled = settings.getValue("syphonServersEnabled", false);
    
    if(syphonServersEnabled) {
        layerOutputSyphonServers.resize(numLayers+1);
        for(int i = 1; i <= numLayers; i++) {
            layerOutputSyphonServers[i].setName(settings.getValue("syphonServersName","")+ofToString(i));
        }
    }
    
    hand.loadImage("resources/hand.png");
    
}
void PuppetsHandler::update(LeapDataHandler *leap,
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
        }
        
        // update puppet
        puppets[i].update();
        
    }
    
    // record puppets
    if(recording) {
        
        newRecording.addFrame(puppets[recordingPuppetIndex].getDeformedMesh(),
                              puppets[recordingPuppetIndex].getPosition(),
                              puppets[recordingPuppetIndex].getRotation());
        
    }
    
}
void PuppetsHandler::draw(LeapDataHandler *leap) {
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    // draw puppets
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for(int i = 0; i < puppets.size(); i++) {
        if(activeLayer == puppets[i].getLayer()) {
            bool isSelected = i == selectedPuppetIndex;
            puppets[i].draw(isSelected,recording);
        }
    }
    
    // draw currently selected vertex info
    if(selectedPuppet() != NULL && selectedVertexIndex != -1) {
        ofColor blinkColor = SELECTED_VERTEX_COLOR;
        blinkColor.a = 150+sin(ofGetElapsedTimef()*10)*100;
        ofSetColor(blinkColor);
        ofCircle(selectedPuppet()->getDeformedMesh().getVertex(selectedVertexIndex), SELECTED_VERTEX_RADIUS);
    }
    
    // highlight the vertex that the mouse is hovered over
    if(hoveredVertexIndex != -1) {
        
        ofSetColor(HOVERED_VERTEX_BORDER_COLOR);
        ofCircle(selectedPuppet()->getDeformedMesh().getVertex(hoveredVertexIndex), VERTEX_BORDER_RADIUS);
        
        ofSetColor(HOVERED_VERTEX_COLOR);
        ofCircle(selectedPuppet()->getDeformedMesh().getVertex(hoveredVertexIndex), VERTEX_RADIUS);
    }
    
    // draw bones
    if(addingBone && hoveredVertexIndex != -1) {
        ofVec3f fromVertex = selectedPuppet()->getDeformedMesh().getVertex(boneRootVertexIndex);
        ofVec3f toVertex = selectedPuppet()->getDeformedMesh().getVertex(hoveredVertexIndex);
        
        ofSetColor(BONE_COLOR);
        ofSetLineWidth(BONE_LINE_WIDTH);
        ofLine(fromVertex.x, fromVertex.y, toVertex.x, toVertex.y);
        ofSetLineWidth(1);
    }
    
    ofPopMatrix();
    
    // draw layer previews
    for(int layer = 1; layer <= numLayers; layer++) {
        
        int previewX = ofGetWidth()
                     - (numLayers*LAYER_PREVIEW_WIDTH+LAYER_PREVIEW_WIDTH)
                     + layer*LAYER_PREVIEW_WIDTH;
        
        ofNoFill();
        ofSetColor(255,255,255);
        ofRect(previewX,0,LAYER_PREVIEW_WIDTH,LAYER_PREVIEW_HEIGHT);
        
        ofFill();
        if(layer == activeLayer) {
            ofSetColor(ACTIVE_LAYER_PREVIEW_COLOR);
            ofRect(previewX,0,LAYER_PREVIEW_WIDTH,LAYER_PREVIEW_HEIGHT);
        }
        
        ofSetColor(255,255,255);
        ofDrawBitmapString(ofToString(layer),
                           previewX + LAYER_PREVIEW_TEXT_OFFSET,
                           LAYER_PREVIEW_HEIGHT - LAYER_PREVIEW_TEXT_OFFSET);
        
        ofPushMatrix();
        ofTranslate(previewX+LAYER_PREVIEW_WIDTH/2,
                    LAYER_PREVIEW_HEIGHT/2);
        ofScale(LAYER_PREVIEW_PUPPET_SCALE,
                LAYER_PREVIEW_PUPPET_SCALE);
        
        for(int i = 0; i < puppets.size(); i++) {
            if(layer == puppets[i].getLayer()) {
                puppets[i].draw(false,false);
            }
        }
        
        ofPopMatrix();
    }
    
}

void PuppetsHandler::publishSyphonOutput() {
    
    if(syphonServersEnabled) {
    
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2,
                    ofGetHeight()/2);
        
        for(int layer = 1; layer <= numLayers; layer++) {
            
            ofBackground(0,0,0,0);
            glEnable(GL_BLEND);
            glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            
            for(int i = 0; i < puppets.size(); i++) {
                if(layer == puppets[i].getLayer()) {
                    puppets[i].draw(false,false);
                }
            }
            
            layerOutputSyphonServers[layer].publishScreen();
            
        }
        
        ofPopMatrix();
        
    }
    
}

void PuppetsHandler::addPuppet(Puppet p) {
    
    p.setLayer(activeLayer);
    puppets.push_back(p);
    
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
                int layer = info.getValue("layer", 1);
                
                if(isControllable) {
                    loadPuppet(path+"/"+puppetDirPath);
                } else {
                    loadRecording(path+"/"+puppetDirPath);
                }
                
                puppets[puppets.size()-1].setLayer(layer);
                
                info.popTag();
                
            }
        
        }
    }
    
}
void PuppetsHandler::exportScene() {
    
    ofFileDialogResult saveFileResult = ofSystemSaveDialog(DEFAULT_SCENE_NAME, "Select location to export scene:");
    
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
            info.addValue("layer", puppets[i].getLayer());
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
            
            if(puppets[clickedPuppetIndex].getLayer() == activeLayer) {
                hoveredVertexIndex = -1;
                selectedPuppetIndex = clickedPuppetIndex;
                selectedVertexIndex = -1;
            }
            
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
            VERTEX_SELECTION_RADIUS
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
    
    ofFileDialogResult saveFileResult = ofSystemSaveDialog(DEFAULT_PUPPET_NAME, "Select location to export puppet:");
    
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
        
        for(int i = 0; i < puppets.size(); i++) {
            if(puppets[i].isControllable()) {
                recordingPuppetIndex = i;
            }
        }
         
        newRecording.setImage(puppets[recordingPuppetIndex].getImage());
        newRecording.clearCachedFrames();
        newRecording.makeRecording();
        
    } else {
        
        addPuppet(newRecording);
        recording = false;
        
    }
    
}

void PuppetsHandler::setActiveLayer(int l) {
    
    if(l <= numLayers) {
        activeLayer = l;
        
        if(isAPuppetSelected()) {
            selectedPuppet()->setLayer(activeLayer);
        }
    }
    
}
int PuppetsHandler::getActiveLayer() {
    
    return activeLayer;
    
}
int PuppetsHandler::getNumLayers() {
    return numLayers;
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
                                                    VERTEX_SELECTION_RADIUS);
        
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
        
        leapClickAgainTimer = LEAP_CLICK_DELAY;
        
    }
    if(leapClickAgainTimer > 0) {
        leapClickAgainTimer--;
    }
    
}
