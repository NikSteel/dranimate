#ifndef __dranimate__Settings__
#define __dranimate__Settings__

#include <stdio.h>

#include "ofxXmlSettings.h"
#include "ofxGui.h"

class Settings {
    
public:
    
    void loadSettingsXML();
    void saveSettingsXML();
    
    void setup();
    void update();
    void draw();
    
    ofxPanel gui;
    
    ofxToggle flipVertical;
    ofxToggle flipHorizontal;
    ofxToggle invertImage;
    ofxSlider<int> backgroundBrightness;
    
    
};

#endif