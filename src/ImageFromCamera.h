#ifndef __dranimate__ImageFromCamera__
#define __dranimate__ImageFromCamera__

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

#endif