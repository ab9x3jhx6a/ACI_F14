#include "Particle.h"




Particle::Particle(){
	//Particle(ofVec2f(0,0), ofVec2f(0,0), ofColor::white);
}


Particle::Particle(ofVec2f pos, ofVec2f vel, int r, int g, int b){
	position = pos;
	velocity = vel;
	red = r;
	green = g;
	blue = b;
	destroy = false;
}

void Particle::draw(){
	ofSetColor(red,green,blue);
	int ran = ofRandom(10,20);
	if (ran > 15) ran = 1;
	else ran = -1;
	int vibrate = ofRandom(0,3);
	ofCircle(position+ran*vibrate, 1.0);
	
}

void Particle::update(){
	velocity.limit(40.0);
	position += velocity;

	if(position.y < -384 || position.y > 384) {
		destroy = true;
	}
	if(position.x < -512 || position.x > 512) {
		destroy = true;
	}


}

void Particle::applyForces(){
	ofVec2f gravity = ofVec2f((position.x*position.x)/15000,(position.y*position.y)/15000);
	velocity -= gravity;
}

bool Particle::death(){
	if (destroy==true)
		return true;
	else return false;
}
