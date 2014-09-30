#include "ofApp.h"

#include <string>
#include <vector>

//--------------------------------------------------------------

void ofApp::setup(){
	ofSetFrameRate(30);
	r=50;
	g=50;
	b=50;
	sr=1;
	sg=1;
	sb=1;	
	deg = 10;
	num_song = 0;

	songs.push_back("Somewhere_I_belong.mp3");
	songs.push_back("i.mp3");
	songs.push_back("Rnb.mp3");

	disk.loadSound(songs[num_song]);
	disk.play();
	disk.setLoop(true);
	
	
	scratch.loadSound("dj_Scratch.mp3");
	applause.loadSound("Applause.mp3");
	dj.loadImage("disk.png");
	pause = false;

	

}
	
void rotatedisk(bool pause, int& deg) {
	ofTranslate(500,400);
	if (!pause){
		ofRotate(deg,1,1,0);
		deg++;
	}
	else
		ofRotate(deg,1,1,0);
	if(deg == 360)
		deg = 0;
}


void backgroundshift(int&r,int&sign) {
	int rand;
	
	rand = ofRandom(2,6);

	if(r>220||r<50){
		sign = -sign;
	}

	r += sign*rand;
}

//--------------------------------------------------------------
void ofApp::update(){
	

	backgroundshift(r,sr);
	backgroundshift(g,sg);
	backgroundshift(b,sb);
	ofBackground(r,g,b);
	

	vector<Particle> hold;
	for (int i=0; i<emitter.size();i++){
		emitter[i].update();
		if(!emitter[i].death())
			hold.push_back(emitter[i]);
	}
	emitter.clear();
	emitter = hold;
	hold.clear();

}

//--------------------------------------------------------------
void ofApp::draw(){	
	rotatedisk(pause, deg);
	dj.draw(-200,-200,400,400);

	for (int i=0; i<emitter.size(); i++) {
		emitter[i].draw();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	num_song++;
	if(num_song > 2)
		num_song = 0;
	disk.loadSound(songs[num_song]);
	disk.play();
	disk.setLoop(true);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	
	int max = ofRandom(10,20);
	for(int i=0; i<max; i++) {
		emitter.push_back(Particle(
			ofVec2f(x-500,y-400), ofVec2f(ofRandom(-20,20), ofRandom(-20,20)), (255-r), (255-g), (255-b)));
	}
	int prev = disk.getPositionMS();

	int mpx = ofGetPreviousMouseX();
	int mpy = ofGetPreviousMouseY();
	int dist = sqrt((mpx-x)*(mpx-x) + (mpy-y)*(mpy-y));
	int rand = ofRandom(10,20);
	if (rand<15) rand = -1;
	else rand = 1;
	disk.setPositionMS(prev+rand*dist*20);
	scratch.setSpeed(0.8+dist/20);

	int scr = sqrt((mpx-x)*(mpx-x) + (mpy-y)*(mpy-y));
	pause = true;
	if((mpx-x)<0) deg+=scr/5;
	else deg-=scr/5;
	rotatedisk(pause, deg);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	scratch.play();
	scratch.setLoop(true);
	pause = true;
	applause.play();
	int max = ofRandom(10,20);
	for(int i=0; i<max; i++) {
		emitter.push_back(Particle(
			ofVec2f(x,y), ofVec2f(ofRandom(-20,20), ofRandom(-20,20)), (255-r), (255-g), (255-b)));
	}
	disk.setPaused(true);

	mousex = x;
	mousey = y;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	scratch.stop();
	pause = false;
	disk.setPaused(false);
	disk.setSpeed(1);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

