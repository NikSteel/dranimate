#pragma once

#include <stdio.h>

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Global.h"

class ImageFromCamera {
    
public:
    
    void setup();
    void update();
    
    ofVideoGrabber vidGrabber;
    unsigned char *videoPixels;
    ofTexture videoTexture;
    int camWidth;
    int camHeight;
    
    ofImage image;
    
};
