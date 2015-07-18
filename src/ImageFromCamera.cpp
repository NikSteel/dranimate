#include "ImageFromCamera.h"

void ImageFromCamera::setup() {
    
    ofxXmlSettings settings; settings.load(LAUNCH_SETTINGS_PATH);
    
    camWidth = settings.getValue("cameraCaptureWidth", 0);
    camHeight = settings.getValue("cameraCaptureHeight", 0);
    
    vidGrabber.setDeviceID(settings.getValue("cameraDeviceID", 0));
    vidGrabber.setDesiredFrameRate(settings.getValue("cameraCaptureFramerate", 0));
    vidGrabber.initGrabber(camWidth,camHeight);
    
    videoTexture.allocate(camWidth,camHeight, GL_RGB);
    
    image.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
    
}
void ImageFromCamera::update() {
    
    vidGrabber.update();
    
    if (vidGrabber.isFrameNew()){
        unsigned char * pixels = vidGrabber.getPixels();
        videoTexture.loadData(pixels, camWidth,camHeight, GL_RGB);
        
        image.setFromPixels(vidGrabber.getPixelsRef());
        
    }
    
}