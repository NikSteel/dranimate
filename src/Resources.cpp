#include "Resources.h"

ofImage Resources::hand;
ofImage Resources::grid;
ofTrueTypeFont Resources::verdana24;
ofTrueTypeFont Resources::verdana16;
ofTrueTypeFont Resources::verdana12;

void Resources::loadResources() {
    
    Resources::hand.loadImage("resources/hand.png");
    Resources::grid.loadImage("resources/grid.png");
    
    Resources::verdana24.loadFont("resources/WeibeiTC-Bold.otf", 24, true, true);
    Resources::verdana24.setLineHeight(34.0f);
    Resources::verdana24.setLetterSpacing(1.035);
    
    Resources::verdana16.loadFont("resources/WeibeiTC-Bold.otf", 16, true, true);
    Resources::verdana16.setLineHeight(18.0f);
    Resources::verdana16.setLetterSpacing(1.035);
    
    Resources::verdana12.loadFont("resources/WeibeiTC-Bold.otf", 12, true, true);
    Resources::verdana12.setLineHeight(14.0f);
    Resources::verdana12.setLetterSpacing(1.035);
    
}