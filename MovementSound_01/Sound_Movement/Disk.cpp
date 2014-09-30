
#include "Disk.h"

Disk::Disk() {}

Disk::Disk(ofVec2f pos, float rad){
	position = pos;
	radius = rad;
}

void Disk::draw() {
	ofSetColor(ofColor::black);
	ofCircle(position, radius);
	ofSetColor(ofColor::red);
	ofCircle(position, radius/3);
	ofSetColor(ofColor::white);
	ofCircle(position, radius/10);
}

void Disk::rotate (float deg) {
	ofRotateZ(deg);
}
