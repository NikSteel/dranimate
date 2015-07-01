#include "ofApp.h"

void ofApp::setup() {
    
    // setup all of the different handlers
    
    cam.setup();
    mesher.setup();
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
            
            leapHandler.update();
            
            puppetsHandler.cdMenuOpen = clickDownMenu.phase != PHASE_WAIT;
            puppetsHandler.update(&leapHandler, &oscReceiver, &clickDownMenu);
            
            break;
            
        case LEAP_CALIBRATION:
            leapHandler.update();
            break;
            
    }
    
}

void ofApp::draw() {
    
    ofSetColor(255);
    //ofBackgroundGradient(ofColor(50,50,50), ofColor(25,25,25), OF_GRADIENT_LINEAR);
    ofBackgroundGradient(ofColor(255,255,255), ofColor(210,210,210), OF_GRADIENT_LINEAR);
    
    switch(state) {
            
        case NEW_PUPPET_CREATION: {
        
            Utils::drawGrid();
            
            mesher.draw();
            
            Utils::drawControls("Puppet creator\n\np - Preview mesh\nm - Generate mesh & create puppet");
            
            break;
        
        } case PUPPET_STAGE: {
            
            puppetsHandler.draw();
                        
            // instructions
            
            Utils::drawControls("Puppet stage\n\nc - Calibrate leap contoller\ns - Start/stop scene recording");
            
            if(!leapHandler.calibrated) {
                Utils::drawWarning("Leap not calibrated!");
            } else if (puppetsHandler.controlsPaused) {
                Utils::drawWarning("Controls paused!");
            }
            
            leapHandler.drawLeapCalibrationMenu();
            
            break;
            
        }
            
        case LEAP_CALIBRATION:
            
            leapHandler.drawLeapCalibrationMenu();
            
            Utils::drawControls("c   -   Set calibration\nt   -   Calibrate on timer");
            
            break;
            
    }
    
    clickDownMenu.draw();
    
    ofSetColor(0, 155, 0);
    ofCircle(mouseX, mouseY, 2);
    
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
                
                newPuppet.setImage(mesher.getImage());
                newPuppet.setMesh(mesher.getMesh());
                newPuppet.setContour(mesher.getContour());
                
                puppetsHandler.addPuppet(newPuppet);
                state = PUPPET_STAGE;
            }
            
            break;

        case PUPPET_STAGE:
            
            // switch to leap calibration mode
            if(key == 'c') {
                state = LEAP_CALIBRATION;
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
                leapHandler.calibrationTimer = 100;
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
    
    if(Utils::filenameIsImage(info.files.at(0))) {
        
        // image file to be used for puppet generation dragged into window
        
        newPuppet.reset();
        newPuppet.setImage(info.files.at(0));
        mesher.reset();
        mesher.setImage(newPuppet.image);
        
        createPuppetLiveMode = false;
        state = NEW_PUPPET_CREATION;
        
    } else if(state == PUPPET_STAGE) {
        
        // load puppet - a puppet directory was dragged into window
        
        Puppet loadedPuppet;
        loadedPuppet.load(info.files.at(0));
        puppetsHandler.addPuppet(loadedPuppet);
        state = PUPPET_STAGE;
        
    }
    
}

void ofApp::mousePressed(int x, int y, int button) {
    
    if(state == NEW_PUPPET_CREATION) {
        mesher.addExtraVertex(x,y);
    }
    
    if(clickDownMenu.phase == PHASE_WAIT) {
    
            
    }
    
}

void ofApp::mouseDragged(int x, int y, int button) {
    
    
    
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
    clickDownMenu.UnRegisterMenu("remove ezone");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("edit puppet");
    clickDownMenu.UnRegisterMenu("export puppet");
    clickDownMenu.UnRegisterMenu("remove puppet");
    clickDownMenu.UnRegisterMenu("reset puppet");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("export recording as mov");
    clickDownMenu.UnRegisterMenu("remove recording");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("clear all puppets");
    clickDownMenu.UnRegisterMenu("clear all recordings");
    clickDownMenu.UnRegisterMenu(" ");
    clickDownMenu.UnRegisterMenu("control type");
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
            clickDownMenu.RegisterMenu("clear all puppets");
            clickDownMenu.RegisterMenu("clear all recordings");
            clickDownMenu.RegisterMenu(" ");
            
            vector<string> controlTypes;
            controlTypes.push_back("mouse");
            controlTypes.push_back("leap");
            clickDownMenu.RegisterBranch("control type", &controlTypes);
            clickDownMenu.RegisterMenu(" ");
            
        } else {
            
            if(puppetsHandler.selectedPuppet()->isBeingEdited) {
                
                if(puppetsHandler.hoveredVertexIndex != -1
                   && puppetsHandler.selectedVertexIndex !=
                   puppetsHandler.hoveredVertexIndex) {
                    
                    // a vertex is hovered over
                    clickDownMenu.RegisterMenu("add ezone");
                    clickDownMenu.RegisterMenu(" ");
                    
                }
                
                if(puppetsHandler.selectedVertexIndex != -1
                   && puppetsHandler.selectedVertexIndex == puppetsHandler.hoveredVertexIndex) {
                    
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
                    clickDownMenu.RegisterMenu("remove ezone");
                    
                    clickDownMenu.RegisterMenu(" ");
                }
                
            }
            
            // a puppet is selected
            clickDownMenu.RegisterMenu("edit puppet");
            clickDownMenu.RegisterMenu("export puppet");
            clickDownMenu.RegisterMenu("remove puppet");
            clickDownMenu.RegisterMenu("reset puppet");
            clickDownMenu.RegisterMenu(" ");
            
        }
        
        if(puppetsHandler.selectedRecordingIndex != -1) {
            clickDownMenu.RegisterMenu("export recording as mov");
            clickDownMenu.RegisterMenu("remove recording");
            clickDownMenu.RegisterMenu(" ");
        }
    } else {
        
        clickDownMenu.RegisterMenu("back to stage");
        clickDownMenu.RegisterMenu(" ");
        
    }
}

void ofApp::cmdEvent(ofxCDMEvent &ev){
    
    if (ev.message == "menu::load puppet") {
        
        ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a puppet directory:",true);
        
        if (openFileResult.bSuccess){
            Puppet loadedPuppet;
            loadedPuppet.load(openFileResult.getPath());
            puppetsHandler.addPuppet(loadedPuppet);
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
        mesher.reset();
        newPuppet.reset();
        state = NEW_PUPPET_CREATION;
        
    }
    
    puppetsHandler.recieveMenuCommand(ev.message);
    
    if (ev.message == "menu::back to stage") {
        
        state = PUPPET_STAGE;
        
    }
    
}