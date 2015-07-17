#include "Settings.h"

void Settings::setup() {
    
    gui.setup();
    
    gui.add(backgroundBrightness.setup("backgroundBrightness", 0, 0, 255));
    
    loadSettingsXML();
    
}
void Settings::update() {
    
    
    
}
void Settings::draw() {
    
    gui.draw();
    
}

void Settings::loadSettingsXML() {
    
    ofxXmlSettings settings;
    //settings.load("settings.xml");
    
    
    
}
void Settings::saveSettingsXML() {
    
    
    
}