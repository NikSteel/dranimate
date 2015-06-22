#include "ofApp.h"

void ofApp::setup() {
    
    // setup the mesh generator
    
    cam.setup();
    mesher.setup();
    
    // setup leap stuff
    
    leapHandler.setup();
    
    // load a demo puppet
    
    Puppet demoPuppet;
    demoPuppet.load("exported-puppets/demo-killingashkeboos/");
    puppets.push_back(demoPuppet);
    
    // setup ui stuff
    
    backgroundBrightness = 0.25;
    
    state = PUPPET_STAGE;
    
    hoveredVertexIndex = -1;
    selectedVertexIndex = -1;
    selectedPuppetIndex = -1;

    // setup clickdown menu stuff
    
    clickDownMenu.OnlyRightClick = true;
    clickDownMenu.menu_name = "menu";
    ofAddListener(ofxCDMEvent::MenuPressed, this, &ofApp::cmdEvent);
    
    Utils::loadImages();
    
}

void ofApp::update() {
    
    // this is because of that strange hidden cursor bug... ugh (see the github issue)
    ofShowCursor();
    
    // update which things show up on the click down menu
    updateClickDownMenu();
    
    switch(state) {
        
        case NEW_PUPPET_CREATION:
            
            // if we're using a live fed image, get it from
            // the camera and give it to the meshe generator
            if(createPuppetLiveMode) {
                cam.update();
                newPuppet.setImage(cam.image);
                mesher.setImage(newPuppet.image);
            }
            
            // if not, the mesher already has the user-loaded image
            mesher.update();
            
            break;
            
        case PUPPET_STAGE:
            
            // update where the mouse is pointing
            if(selectedPuppet() != NULL && clickDownMenu.phase == PHASE_WAIT) {
                hoveredVertexIndex = Utils::getClosestIndex(selectedPuppet()->meshDeformer.getDeformedMesh(), mouseX, mouseY);
            }
            
            // update & send new leap data to puppets
            leapHandler.recieveNewData();
            for(int i = 0; i < puppets.size(); i++) {
                if(i != selectedPuppetIndex || recordingPuppet) {
                    puppets[i].recieveLeapData(&leapHandler);
                    puppets[i].update();
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
            
            for(int i = 0; i < recordedPuppets.size(); i++) {
                recordedPuppets[i].update();
            }
            
            break;
            
        case LEAP_CALIBRATION:
            leapHandler.recieveNewData();
            break;
            
    }
    
}

void ofApp::draw() {
    
    ofSetColor(255);
    ofBackground((int)(backgroundBrightness*255));
    
    switch(state) {
            
        case NEW_PUPPET_CREATION: {
        
            mesher.draw();
            
            Utils::drawControls("p   -   Preview mesh\nm   -   Generate mesh and create puppet");
            
            Utils::drawState("Mesh creator");
            
            break;
        
        } case PUPPET_STAGE: {
            
            // state stuff
            
            Utils::drawState("Puppet stage");
            
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
                ofSetColor(ofColor(0,155,255));
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
            
            // instructions
            
            Utils::drawControls("c     -    Calibrate leap contoller\nr     -    Start/stop puppet recording\ns     -    Start/stop whole scene recording\nspace -    Pause all animation");
            
            if(!leapHandler.calibrated) {
                Utils::drawWarning("Leap not calibrated!");
            } else if (wholeScenePaused) {
                Utils::drawWarning("Animation paused!");
            }
            
            break;
            
        }
            
        case LEAP_CALIBRATION:
            
            leapHandler.drawLeapCalibrationMenu();
            
            if(leapHandler.calibrated) {
                Utils::drawWarning("No leap calibration!");
            }
            
            Utils::drawControls("c   -   Set calibration");
            
            break;
            
    }
    
    clickDownMenu.draw();
    
    ofSetColor(255, 255, 255);
    ofCircle(mouseX, mouseY, 2);
    
}

int ofApp::getClosestPuppetIndex() {
    
    int closestPuppetIndex = -1;
    
    for(int p = 0; p < puppets.size(); p++) {
        if(Utils::isPointInsideMesh(puppets[p].meshDeformer.getDeformedMesh(), mouseX,mouseY)) {
            closestPuppetIndex = p;
            break;
        }
    }
    
    return closestPuppetIndex;
    
}

int ofApp::getClosestRecordingIndex() {
    
    int closestRecordingIndex = -1;
    
    for(int p = 0; p < recordedPuppets.size(); p++) {
        if(Utils::isPointInsideMesh(recordedPuppets[p].getCurrentMesh(), mouseX,mouseY)) {
            closestRecordingIndex = p;
            break;
        }
    }
    
    return closestRecordingIndex;
    
}

Puppet* ofApp::selectedPuppet() {
    
    if(selectedPuppetIndex == -1) {
        return NULL;
    } else {
        return &puppets[selectedPuppetIndex];
    }
    
}

string ofApp::getSelectedVertexInfo() {
    
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

void ofApp::keyReleased(int key) {
    
    switch (state) {
            
        case NEW_PUPPET_CREATION:
            
            // preview mesh
            if(key == 'p') {
                mesher.generateMesh();
            }
            
            // finalize mesh and create a new puppet from that mesh
            if(key == 'm') {
                mesher.generateMesh();
                
                newPuppet.setMesh(mesher.getMesh());
                newPuppet.setContour(mesher.getContour());
                
                puppets.push_back(newPuppet);
                state = PUPPET_STAGE;
            }
            
            break;

        case PUPPET_STAGE:
            
            // switch to leap calibration mode
            if(key == 'c') {
                state = LEAP_CALIBRATION;
            }
            
            // delete selected puppet
            if(key == OF_KEY_BACKSPACE) {
                if(selectedPuppet() != NULL) {
                    puppets.erase(puppets.begin() + selectedPuppetIndex);
                    selectedPuppetIndex = -1;
                    selectedVertexIndex = -1;
                    hoveredVertexIndex = -1;
                }
            }
            
            // toggle puppet pause
            if(key == ' ') {
                wholeScenePaused = !wholeScenePaused;
                for(int i = 0; i < puppets.size(); i++) {
                    puppets[i].isPaused = wholeScenePaused;
                }
                for(int i = 0; i < recordedPuppets.size(); i++) {
                    recordedPuppets[i].isPaused = wholeScenePaused;
                }
            }
            
            // toggle puppet recording
            if(key == 'r') {
                if(!recordingPuppet) {
                    puppetRecorder.setup();
                    recordingPuppet = true;
                } else {
                    recordedPuppets.push_back(puppetRecorder);
                    recordingPuppet = false;
                }
            }
            
            // toggle scene recording
            if(key == 's') {
                if(!recordingScene) {
                    sceneRecorder.setup();
                    recordingScene = true;
                } else {
                    sceneRecorder.exportAsMovie();
                    recordingScene = false;
                }
            }
            
            break;
            
        case LEAP_CALIBRATION:
            
            // set leap calibration
            if(key == 'c') {
                leapHandler.calibrate();
                state = PUPPET_STAGE;
            }
            
            break;
            
        default:
            break;
            
    }
    
    // toggle fullscreen
    if(key == 'f') {
        ofToggleFullscreen();
    }
    
}

// lets us drag an image/puppet directory into the window to load it - very convenient

void ofApp::dragEvent(ofDragInfo info) {
    
    switch (state) {
            
        case NEW_PUPPET_CREATION:
            
            break;
            
        case PUPPET_STAGE:
            
            if(info.files.size() > 0) {
               
                if(Utils::filenameIsImage(info.files.at(0))) {
                    
                     // image file to be used for puppet generation dragged into window
                    
                    newPuppet.reset();
                    newPuppet.setImage(info.files.at(0));
                    mesher.setImage(newPuppet.image);
                    state = NEW_PUPPET_CREATION;
                    
                } else {
                    
                    // new puppet - a puppet directory was dragged into window
                    
                    Puppet loadedPuppet;
                    loadedPuppet.load(info.files.at(0));
                    puppets.push_back(loadedPuppet);
                    state = PUPPET_STAGE;
                    
                }
                
            }
            
            break;
            
        default:
            break;
            
    }
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    if(state == NEW_PUPPET_CREATION) {
        mesher.addExtraVertex(x,y);
    }
    
    if(clickDownMenu.phase == PHASE_WAIT) {
    
    if(button == 2) {
        selectedRecordingIndex = getClosestRecordingIndex();
    }
        
    int clickedPuppetIndex = getClosestPuppetIndex();
    if(clickedPuppetIndex != selectedPuppetIndex && hoveredVertexIndex == -1) {
        
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

void ofApp::mouseDragged(int x, int y, int button) {
    
    if(selectedVertexIndex != -1) {
        
        //selectedPuppet()->meshDeformer.setControlPoint(selectedVertexIndex, ofVec2f(x,y));
        selectedPuppet()->mesh.setVertex(selectedVertexIndex, ofVec2f(x,y)-selectedPuppet()->getExpressionZone(selectedVertexIndex)->userControlledDisplacement);
        selectedPuppet()->update();
        
    }
    
}

void ofApp::updateClickDownMenu() {
    
    // reset everything
    clickDownMenu.UnRegisterMenu("load puppet");
    clickDownMenu.UnRegisterMenu("create puppet");
    clickDownMenu.UnRegisterMenu("create puppet (live)");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("add ezone");
    clickDownMenu.UnRegisterMenu("add leap mapping");
    clickDownMenu.UnRegisterMenu("add osc mapping");
    clickDownMenu.UnRegisterMenu("add bone");
    clickDownMenu.UnRegisterMenu("remove ezone");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("reset pose");
    clickDownMenu.UnRegisterMenu("scale puppet");
    clickDownMenu.UnRegisterMenu("pause/unpause puppet");
    clickDownMenu.UnRegisterMenu("export puppet");
    clickDownMenu.UnRegisterMenu("remove puppet");
    clickDownMenu.UnRegisterMenu("reset puppet");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("export recording as mov");
    clickDownMenu.UnRegisterMenu("remove recording");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("bg brightness");
    clickDownMenu.UnRegisterMenu("leap sensitivity");
    clickDownMenu.UnRegisterMenu("clear all");
    clickDownMenu.UnRegisterMenu(" ");
    
    if(selectedPuppet() == NULL) {
        
        // no puppet is selected
        clickDownMenu.RegisterMenu("load puppet");
        clickDownMenu.RegisterMenu("create puppet");
        clickDownMenu.RegisterMenu("create puppet (live)");
        clickDownMenu.RegisterMenu(" ");
        clickDownMenu.RegisterFader("bg brightness", &backgroundBrightness);
        clickDownMenu.RegisterFader("leap sensitivity", &leapHandler.sensitivity);
        clickDownMenu.RegisterMenu("clear all");
        clickDownMenu.RegisterMenu(" ");
        
    } else {
        
        if(hoveredVertexIndex != -1 && selectedVertexIndex != hoveredVertexIndex) {
            // a vertex is hovered over
            clickDownMenu.RegisterMenu("add ezone");
            clickDownMenu.RegisterMenu(" ");
        }
        
        if(selectedVertexIndex != -1 && selectedVertexIndex == hoveredVertexIndex) {
            // a vertex is selected
            
            vector<string> leapFingersBranch;
            leapFingersBranch.push_back("none");
            leapFingersBranch.push_back("thumb");
            leapFingersBranch.push_back("index");
            leapFingersBranch.push_back("middle");
            leapFingersBranch.push_back("ring");
            leapFingersBranch.push_back("pinky");
            
            clickDownMenu.RegisterBranch("add leap mapping", &leapFingersBranch);
            clickDownMenu.RegisterMenu("add osc mapping");
            clickDownMenu.RegisterMenu("add bone");
            clickDownMenu.RegisterMenu("remove ezone");
            
            clickDownMenu.RegisterMenu(" ");
        }
        
        // a puppet is selected
        clickDownMenu.RegisterMenu("reset pose");
        clickDownMenu.RegisterFader("scale puppet", &selectedPuppet()->scale);
        clickDownMenu.RegisterMenu("pause/unpause puppet");
        clickDownMenu.RegisterMenu("export puppet");
        clickDownMenu.RegisterMenu("remove puppet");
        clickDownMenu.RegisterMenu("reset puppet");
        clickDownMenu.RegisterMenu(" ");
        
    }
    
    if(selectedRecordingIndex != -1) {
        clickDownMenu.RegisterMenu("export recording as mov");
        clickDownMenu.RegisterMenu("remove recording");
        clickDownMenu.RegisterMenu(" ");
    }
    
}

void ofApp::cmdEvent(ofxCDMEvent &ev){
    
    if (ev.message == "menu::load puppet") {
        
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a puppet directory:",true);
        
        if (openFileResult.bSuccess){
            Puppet loadedPuppet;
            loadedPuppet.load(openFileResult.getPath());
            puppets.push_back(loadedPuppet);
            state = PUPPET_STAGE;
        }
        
    }
    if (ev.message == "menu::create puppet") {
        
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image:",true);
        
        if (openFileResult.bSuccess){
            newPuppet.reset();
            newPuppet.setImage(openFileResult.getPath());
            mesher.reset();
            mesher.setImage(newPuppet.image);
            
            createPuppetLiveMode = false;
            state = NEW_PUPPET_CREATION;
        }
        
    }
    if (ev.message == "menu::create puppet (live)") {
        
        createPuppetLiveMode = true;
        newPuppet.reset();
        state = NEW_PUPPET_CREATION;
        
    }
    if (ev.message == "menu::add ezone") {
        
        ExpressionZone* eZone = selectedPuppet()->getExpressionZone(hoveredVertexIndex);
        
        if(eZone == NULL) {
            // if there's no expression zone where we clicked, add one.
            selectedPuppet()->addExpressionZone(hoveredVertexIndex);
            selectedVertexIndex = hoveredVertexIndex;
        }
        
    }
    if (ev.message == "menu::add leap mapping::none") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = -1;
    }
    if (ev.message == "menu::add leap mapping::thumb") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 0;
    }
    if (ev.message == "menu::add leap mapping::index") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 1;
    }
    if (ev.message == "menu::add leap mapping::middle") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 2;
    }
    if (ev.message == "menu::add leap mapping::ring") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 3;
    }
    if (ev.message == "menu::add leap mapping::pinky") {
        selectedPuppet()->getExpressionZone(selectedVertexIndex)->leapFingerID = 4;
    }
    if (ev.message == "menu::add osc mapping") {
        
        if(selectedPuppet()->getExpressionZone(selectedVertexIndex) != NULL) {
            OSCNamespace namesp;
            namesp.message = ofSystemTextBoxDialog("osc message?");
            namesp.controlType = ofSystemTextBoxDialog("control type?");
            selectedPuppet()->getExpressionZone(selectedVertexIndex)->oscNamespaces.push_back(namesp);
            
        }
        
    }
    if (ev.message == "menu::add bone") {
        
        addingBone = true;
        boneRootVertexIndex = selectedVertexIndex;
        
    }
    if (ev.message == "menu::remove ezone") {
        
        selectedPuppet()->removeExpressionZone(selectedVertexIndex);
        selectedVertexIndex = -1;
        
    }
    if (ev.message == "menu::reset pose") {
        
        selectedPuppet()->resetPose();
        
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    if (ev.message == "menu::pause/unpause puppet") {
        
        selectedPuppet()->isPaused = !selectedPuppet()->isPaused;
        
    }
    if (ev.message == "menu::export puppet") {
     
        ofFileDialogResult saveFileResult = ofSystemSaveDialog("newpuppet", "Select location to export puppet:");
        
        if (saveFileResult.bSuccess){
            string path = saveFileResult.getPath();
            selectedPuppet()->save(path);
        }
        
    }
    if (ev.message == "menu::remove puppet") {
        
        puppets.erase(puppets.begin() + selectedPuppetIndex);
        
        selectedPuppetIndex = -1;
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    if (ev.message == "menu::reset puppet") {
        
        selectedPuppet()->reset();
        
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    if (ev.message == "menu::export recording as mov") {
        
        recordedPuppets[selectedRecordingIndex].exportAsMovie();
        
    }
    if (ev.message == "menu::remove recording") {
        
        recordedPuppets.erase(recordedPuppets.begin() + selectedRecordingIndex);
        
    }
    if (ev.message == "menu::clear all") {
        
        puppets.clear();
        
        selectedPuppetIndex = -1;
        selectedVertexIndex = -1;
        hoveredVertexIndex = -1;
        
    }
    
}