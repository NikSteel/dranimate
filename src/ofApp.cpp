#include "ofApp.h"

void ofApp::setup() {
    
    // setup all of the different handlers
    
    cam.setup();
    leapHandler.setup();
    oscReceiver.setup(8000);
    puppetsHandler.setup();
    
    // setup ui stuff
    
    state = PUPPET_STAGE;

    clickDownMenu.OnlyRightClick = true;
    clickDownMenu.menu_name = "menu";
    ofAddListener(ofxCDMEvent::MenuPressed, this, &ofApp::cmdEvent);
    
    // load resources
    
    Resources::loadResources();
    
}
void ofApp::update() {
    
    // this is because of that strange hidden cursor bug... ugh (see the github issue)
    ofShowCursor();
    
    // update which things show up on the click down menu
    updateClickDownMenu();
    
    switch(state) {
        
        case NEW_PUPPET_CREATION:
            
            mesher.update();
            
            break;
            
        case PUPPET_STAGE:
            
            leapHandler.update();
            
            puppetsHandler.update(&leapHandler, &oscReceiver, &clickDownMenu);
            
            break;
            
        case LEAP_CALIBRATION:
            leapHandler.update();
            break;
            
    }
    
}
void ofApp::draw() {
    
    ofSetColor(255);
    ofBackground(255,255,255);
    
    switch(state) {
            
        case NEW_PUPPET_CREATION: {
        
            mesher.draw();
            
            break;
        
        } case PUPPET_STAGE: {
            
            puppetsHandler.draw(&leapHandler);
            leapHandler.draw(false);
            
            break;
            
        }
            
        case LEAP_CALIBRATION:
            
            leapHandler.draw(true);
            
            break;
            
    }
    
    clickDownMenu.draw();
    
    // temporary until mouse hiding bug is fixed
    ofSetColor(0, 155, 0);
    ofCircle(mouseX, mouseY, 4);
    
}

void ofApp::keyReleased(int key) {
    
    switch (state) {
            
        case NEW_PUPPET_CREATION:
            
            break;

        case PUPPET_STAGE:
            
            // switch to leap calibration mode
            if(key == 'c') {
                state = LEAP_CALIBRATION;
            }
            
            // swap pointing hand and puppeteering hand
            if(key == OF_KEY_TAB) {
                leapHandler.swapHandControls();
            }
            
            break;
            
        case LEAP_CALIBRATION:
            
            // set leap calibration
            if(key == 'c') {
                leapHandler.calibrate();
                state = PUPPET_STAGE;
            }
            
            // set leap calibration (on timer)
            if(key == 't') {
                leapHandler.calibrationTimer = leapHandler.calibrationTimerLength;
            }
            
            break;
            
        default:
            break;
            
    }
    
    // toggle fullscreen
    if(key == 'f') {
        ofToggleFullscreen();
    }
    
    if(key == 'r') {
        puppetsHandler.togglePuppetRecording();
    }
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    if(state == NEW_PUPPET_CREATION) {
        mesher.addExtraVertex(x,y);
    }
    
    if(clickDownMenu.phase == PHASE_WAIT) {

        puppetsHandler.clickMouseAt(x,y);
        
    }
    
}
void ofApp::mouseMoved(int x, int y) {
    
    if(clickDownMenu.phase == PHASE_WAIT) {
        puppetsHandler.updateWhichVertexIsHoveredOver(x,y);
    }
    
}
void ofApp::mouseDragged(int x, int y, int button) {
    
    
    
}

void ofApp::dragEvent(ofDragInfo info) {
    
    // lets us drag an image/puppet directory into the window to load it - very convenient
    
    if(Utils::filenameIsImage(info.files.at(0))) {
        
        // image file to be used for puppet generation dragged into window
        
        mesher.setup(false);
        mesher.setImage(info.files.at(0));
        
        state = NEW_PUPPET_CREATION;
        
    } else if(state == PUPPET_STAGE) {
        
        // load puppet - a puppet directory was dragged into window
        
        puppetsHandler.loadPuppet(info.files.at(0));
        state = PUPPET_STAGE;
        
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
    clickDownMenu.UnRegisterMenu("set anchor point");
    clickDownMenu.UnRegisterMenu("delete ezone");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("edit puppet");
    clickDownMenu.UnRegisterMenu("export puppet");
    clickDownMenu.UnRegisterMenu("delete puppet");
    clickDownMenu.UnRegisterMenu("reset puppet");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("export recording as mov");
    clickDownMenu.UnRegisterMenu("delete recording");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("new stage");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("preview mesh");
    clickDownMenu.UnRegisterMenu("generate mesh and create puppet");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("back to stage");
    clickDownMenu.UnRegisterMenu(" ");
    
    if(state == PUPPET_STAGE) {
        
        if(puppetsHandler.selectedPuppet() == NULL) {
            
            // no puppet is selected
            clickDownMenu.RegisterMenu("load puppet");
            clickDownMenu.RegisterMenu("create puppet");
            clickDownMenu.RegisterMenu("create puppet (live)");
            clickDownMenu.RegisterMenu(" ");
            clickDownMenu.RegisterMenu("new stage");
            clickDownMenu.RegisterMenu(" ");
            
        } else {
            
            if(puppetsHandler.selectedPuppet()->isBeingEdited) {
                
                if(puppetsHandler.emptyVertexHoveredOver()) {
                    
                    // a vertex is hovered over
                    clickDownMenu.RegisterMenu("add ezone");
                    clickDownMenu.RegisterMenu(" ");
                    
                }
                
                if(puppetsHandler.ezoneHoveredOver()) {
                    
                    // a vertex is selected
                    
                    vector<string> leapFingersBranch;
                    leapFingersBranch.push_back("none");
                    leapFingersBranch.push_back("");
                    leapFingersBranch.push_back("thumb  (hand 1)");
                    leapFingersBranch.push_back("index  (hand 1)");
                    leapFingersBranch.push_back("middle (hand 1)");
                    leapFingersBranch.push_back("ring   (hand 1)");
                    leapFingersBranch.push_back("pinky  (hand 1)");
                    leapFingersBranch.push_back("");
                    leapFingersBranch.push_back("thumb  (hand 2)");
                    leapFingersBranch.push_back("index  (hand 2)");
                    leapFingersBranch.push_back("middle (hand 2)");
                    leapFingersBranch.push_back("ring   (hand 2)");
                    leapFingersBranch.push_back("pinky  (hand 2)");
                    
                    clickDownMenu.RegisterBranch("add leap mapping", &leapFingersBranch);
                    clickDownMenu.RegisterMenu("add osc mapping");
                    clickDownMenu.RegisterMenu("add bone");
                    clickDownMenu.RegisterMenu("set anchor point");
                    clickDownMenu.RegisterMenu("delete ezone");
                    
                    clickDownMenu.RegisterMenu(" ");
                }
                
            }
            
            // a puppet is selected
            clickDownMenu.RegisterMenu("edit puppet");
            clickDownMenu.RegisterMenu("export puppet");
            clickDownMenu.RegisterMenu("delete puppet");
            clickDownMenu.RegisterMenu("reset puppet");
            clickDownMenu.RegisterMenu(" ");
            
        }
        
        /*
        if(puppetsHandler.selectedRecordingIndex != -1) {
            clickDownMenu.RegisterMenu("export recording as mov");
            clickDownMenu.RegisterMenu("remove recording");
            clickDownMenu.RegisterMenu(" ");
        }
         */
    } else {
        
        clickDownMenu.RegisterMenu("preview mesh");
        clickDownMenu.RegisterMenu("generate mesh and create puppet");
        clickDownMenu.RegisterMenu(" ");
        clickDownMenu.RegisterMenu("back to stage");
        clickDownMenu.RegisterMenu(" ");
        
    }
}
void ofApp::cmdEvent(ofxCDMEvent &ev){
    
    if (ev.message == "menu::load puppet") {
        
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a puppet directory:",true);
        
        if (openFileResult.bSuccess){
            puppetsHandler.loadPuppet(openFileResult.getPath());
            state = PUPPET_STAGE;
        }
        
    }
    if (ev.message == "menu::create puppet") {
        
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an image:",true);
        
        if (openFileResult.bSuccess){
            mesher.setup(false);
            mesher.setImage(openFileResult.getPath());
            
            state = NEW_PUPPET_CREATION;
        }
        
    }
    if (ev.message == "menu::create puppet (live)") {
        
        mesher.setup(true);
        state = NEW_PUPPET_CREATION;
        
    }
    
    if (ev.message == "menu::add ezone") {
        
        puppetsHandler.addExpressionZoneToCurrentPuppet();
        
    }
    if (ev.message == "menu::add leap mapping::none") {
        puppetsHandler.addLeapMappingToCurrentPuppet(-1);
    }
    if (ev.message == "menu::add leap mapping::thumb  (hand 1)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(0);
    }
    if (ev.message == "menu::add leap mapping::index  (hand 1)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(1);
    }
    if (ev.message == "menu::add leap mapping::middle (hand 1)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(2);
    }
    if (ev.message == "menu::add leap mapping::ring   (hand 1)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(3);
    }
    if (ev.message == "menu::add leap mapping::pinky  (hand 1)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(4);
    }
    if (ev.message == "menu::add leap mapping::thumb  (hand 2)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(5);
    }
    if (ev.message == "menu::add leap mapping::index  (hand 2)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(6);
    }
    if (ev.message == "menu::add leap mapping::middle (hand 2)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(7);
    }
    if (ev.message == "menu::add leap mapping::ring   (hand 2)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(8);
    }
    if (ev.message == "menu::add leap mapping::pinky  (hand 2)") {
        puppetsHandler.addLeapMappingToCurrentPuppet(9);
    }
    if (ev.message == "menu::add osc mapping") {
        
        puppetsHandler.addOSCMappingToCurrentPuppet();
        
    }
    if (ev.message == "menu::add bone") {
        
        puppetsHandler.addBoneToCurrentPuppet();
        
    }
    if (ev.message == "menu::set anchor point") {
        
        puppetsHandler.setAnchorPointOnCurrentPuppet();
        
    }
    if (ev.message == "menu::delete ezone") {
        
        puppetsHandler.removeEZoneFromCurrentPuppet();
        
    }
    
    if (ev.message == "menu::export puppet") {
        
        puppetsHandler.exportCurrentPuppet();
        
    }
    if (ev.message == "menu::edit puppet") {
        
        puppetsHandler.editCurrentPuppet();
        
    }
    if (ev.message == "menu::delete puppet") {
        
        puppetsHandler.removeCurrentPuppet();
        
    }
    if (ev.message == "menu::reset puppet") {
        
        puppetsHandler.resetCurrentPuppet();
        
    }
    if (ev.message == "menu::export recording as mov") {
        
        puppetsHandler.exportCurrentPuppetRecordingAsMov();
        
    }
    
    if (ev.message == "menu::new stage") {
        
        puppetsHandler.clearAllPupets();
        
    }
    
    // preview mesh
    if (ev.message == "menu::preview mesh") {
        mesher.generateMesh();
    }
    
    // finalize mesh and create a new puppet from that mesh
    if (ev.message == "menu::generate mesh and create puppet") {
        mesher.generateMesh();
        
        Puppet newPuppet;
        newPuppet.setImage(mesher.getImage());
        newPuppet.setMesh(mesher.getMesh());
        newPuppet.addCenterpoint();
        
        puppetsHandler.addPuppet(newPuppet);
        state = PUPPET_STAGE;
    }
    
    if (ev.message == "menu::back to stage") {
        
        state = PUPPET_STAGE;
        
    }
    
}