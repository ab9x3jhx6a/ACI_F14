#include "ofApp.h"


//--------------------------------------------------------------
//helper function
void color_shift(int& r, int& g, int& b, bool& rp, bool& gp, bool& bp) {
	if(r<255&&rp) r+=6;
	if(g<255&&gp) g+=6;
	if(b<255&&bp) b+=6;

	if(r>137&&!rp) r-=6;
	if(g>137&&!gp) g-=6;
	if(b>137&&!bp) b-=6;

	if(r>255) r = 255;
	if(g>255) g = 255;
	if(b>255) b = 255;
	if(r<137) r = 137;
	if(g<137) g = 137;
	if(b<137) b = 137;
	
}


void testApp::setup(){	 

    // Global openFrameworks settings
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    ofLogLevel(OF_LOG_NOTICE);    
    ofSetFrameRate(120.0f);
        
    // init of incrementing values
    count = 0.0f;
    audioPan = 0.5f;
    roll = 0.0f;    
    // set background color
    background.set(DARK);
    interp.set(DARK);
    interpAmt = 0.0f;
    hue = background.getHue();
    
    // allocate fbos and images
    scene.allocate(WIDTH, HEIGHT);
    final.allocate(WIDTH, HEIGHT);
    bgFbo.allocate(WIDTH, HEIGHT);
	noise.loadImage("Film_Grain.jpg");
	texture.loadImage("vignette.jpg");
    
    // create circle outline for "random" particles to follow
    ofPoint p(WIDTH/2, HEIGHT/2);
    circle.arc(p,300,300,0,360,40);
    circle = circle.getResampledBySpacing(4);
    
    // arrage particles around circle at random depths
    for (int i=0; i<circle.getVertices().size(); i++) 
        circle.getVertices()[i].z = 0;
    circle.close(); 
        
    // Shader loading
    shader.load("shaders/chrom_abb.vert", "shaders/chrom_abb.frag");
    texturizer.load("shaders/texture.vert", "shaders/texture.frag");
    blur.load("shaders/blur.vert", "shaders/blur.frag");

    // FFT initialization
	fftSmoothed = new float[8192];
	for (int i = 0; i < 8192; i++){
		fftSmoothed[i] = 0;
	}	
	nBandsToGet = 128;
    
	// load song
	music.loadSound("c.mp3");
    music.setVolume(1.0f);
    music.play();
    music.setLoop(true);
    
    /* === Shooting star VBO === */
	/*ofVec3f v[7];
	ofFloatColor c[7];
	const ofIndexType Faces[] = {3, 4, 5, 6, 0, 2, 1};
	// top left
	c[0].r = 0.918f;
	c[0].g = 0.824f;
	c[0].b = 0.25f;
    c[0].a = STAR_ALPHA; 
    // bottom middle
	c[1].r = 1.0f;
	c[1].g = 1.0f;
	c[1].b = 1.0f;    
    c[1].a = STAR_ALPHA;
	// top right
	c[2].r = 0.918f;
	c[2].g = 0.824f;
	c[2].b = 0.25f;
    c[2].a = STAR_ALPHA;
	// bottom left
	c[3].r = 1.0f;
	c[3].g = 1.0f;
	c[3].b = 1.0f;
    c[3].a = 0.0f;
	// bottom right
	c[4].r = 1.0f;
	c[4].g = 1.0f;
	c[4].b = 1.0f;
    c[4].a = 0.0f;
	// middle left
	c[5].r = 1.0f;
	c[5].g = 1.0f;
	c[5].b = 1.0f;
    c[5].a = 0.0f;
	// middle right
	c[6].r = 1.0f;
	c[6].g = 1.0f;
	c[6].b = 1.0f;
    c[6].a = 0.0f;
	
	// top left
	v[0].x = 0.0f;
	v[0].y = 0.0f;
	v[0].z = 0.0f;
	// bottom middle
	v[1].x = 1.5f;
	v[1].y = 70.0f;
	v[1].z = 0.0f;		
	// top right
	v[2].x = 3.0f;
	v[2].y = 0.0f;
	v[2].z = 0.0f;
	// bottom left	
	v[3].x = 0.0f;
	v[3].y = 60.0f;
	v[3].z = 0.0f;
	// bottom right
	v[4].x = 3.0f;
	v[4].y = 60.0f;
	v[4].z = 0.0f;
	// middle left	
	v[5].x = 0.0f;
	v[5].y = 30.0f;
	v[5].z = 0.0f;
	// bottom right
	v[6].x = 3.0f;
	v[6].y = 30.0f;
	v[6].z = 0.0f;
    
	starVbo.setVertexData(&v[0], 7, GL_STATIC_DRAW);
	starVbo.setColorData(&c[0], 7, GL_STATIC_DRAW);
	starVbo.setIndexData(&Faces[0], 7, GL_STATIC_DRAW);*/
 
}


//--------------------------------------------------------------
void testApp::update(){
        
    
    // interpolate speaker direction
    audioPanMax = 0.5f;        
    if (audioPan <= audioPanMax) 
        audioPan += 0.001f;    
    music.setPan(audioPan);
    
    // interpolate between background colours
    if (interpAmt <= 1.0f) 
        interpAmt += 0.01f;
    background.lerp(interp, interpAmt);
    
	// update the sound playing system:
	ofSoundUpdate();	
	float* val = ofSoundGetSpectrum(nBandsToGet);	
    avgSound = 0;
    
    // smooth fft and calc average volume
	for (int i = 0;i < nBandsToGet; i++){		
		fftSmoothed[i] *= 0.96f;
		if (fftSmoothed[i] < val[i]) fftSmoothed[i] = val[i];	
        avgSound += fftSmoothed[i];
	}
    
    // calculate average loudness of the music for "volume"
    avgSound /= nBandsToGet;
    
    // clear out ofPolylines for next fill
    tracer.clear();
    wave.clear();
    
    // increment based on beat of snare drum
    count += fftSmoothed[7];
    // keep spinning those stars
    roll += 2.0f;
    
    // add shooting stars when symbol is hit
   /* if(fftSmoothed[27] > 0.05f) {        
        stars.push_back(ofPoint(ofRandom(1)*WIDTH, ofRandom(1)*HEIGHT, 0));
    } else if (stars.size() > 5) {
        stars.erase(stars.begin()+(int)ofRandom(stars.size()-1));
    }*/
    
    // stretch out star length
    // "sucked into the vortex"
    for (int i=0; i<stars.size(); i++) {
        stars[i].z += 30;
    }
    
    // set new background based on fft "loudness" average
    if (avgSound > 0.07)
        interp.set(MOON);
    else if (avgSound > 0.04)
        interp.set(METAL);
    else if (avgSound > 0.01)
        interp.set(DARKWOOD);
    else
        interp.set(DARK);
}

//--------------------------------------------------------------
void testApp::draw(){
    
    ofSetColor(255, 255);
    
    // texture background using shader
    bgFbo.begin();
    texturizer.begin();       
        ofSetColor(background, 255);	
        ofRect(0, 0, WIDTH, HEIGHT);  
        // pass in noise and texture images
        texturizer.setUniformTexture("uInputTexture1", noise, 0);
        texturizer.setUniformTexture("uInputTexture2", texture, 1);
        texturizer.setUniform2f("uNoiseOffset", ofRandom(WIDTH),  ofRandom(HEIGHT));
        texturizer.setUniform3f("uColor", background.r, background.g, background.b);
        // set alpha for ghosting effect
        texturizer.setUniform1f("uAlpha", 0.2);
        glBegin(GL_QUADS);  
            glTexCoord2f(0, 0);				glVertex2f(0, 0);  
            glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
            glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
            glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
        glEnd();      
    texturizer.end();
    bgFbo.end();
    
    // draw main content to fbo
    scene.begin();		
        ofPushMatrix(); 
            // draw textured background from above
            bgFbo.draw(0, 0, WIDTH, HEIGHT);  
        
            ofSetColor(255, 255);
            // move to center of screen for rotation
            ofTranslate(WIDTH/2, HEIGHT/2);
            // rotate to beat of the snare (fftSmoothed[7])
            ofRotate(count*3);
            // move back to corner to draw
            ofTranslate(-WIDTH/2, -HEIGHT/2); 
            
            // draw scattered points
            vector<ofPoint>vertices = circle.getVertices();
            for (int i=0; i<vertices.size(); i++) {
                ofPoint pt = vertices[i];
                // calculate cyclical position
                pt.x += sin(pt.x + count*0.5)*40;
                pt.y += sin(pt.y + count*0.5)*40;
				pt.z = pt.z;
                ofCircle(pt.x, pt.y, pt.z, 3*fftSmoothed[1]);
            }	
    
            // draw shooting stars
           /* ofPushMatrix();
            for (vector<ofPoint>::iterator i = this->stars.begin(); i != this->stars.end(); ++i) {   
                ofPushMatrix();        
                    ofTranslate(*i);
                    // make star bigger
                    glScalef(5, 5, 10);
                    glPointSize(10.0f);  
                    //ofRotateZ(roll); -> need to set max swing of 30¡
                    ofRotateX(90);
                    // spin towards the back of the scene
                    ofRotateY(roll);
                        // draw VBO
                        starVbo.drawElements(GL_TRIANGLE_STRIP, 7);
                    ofRotateY(90);
                    // overlap other VBO for 3D effect
                    ofTranslate(-1.5, 0, 0);
                        // draw VBO
                        starVbo.drawElements(GL_TRIANGLE_STRIP, 7);
                ofPopMatrix();
            }
            ofPopMatrix();*/
    
            ofSetColor(255, 50);
            // draw mesh lines
        ofPopMatrix();

    
    scene.end(); // end main content fbo 
    
    
    ofClear(0, 0, 0, 1);
    shader.begin();
        // pass in main scene
        shader.setUniformTexture("baseTex", scene.getTextureReference(), 0);
        // shift channels based on bass drum
        shader.setUniform2f("uAberrationOffset", fftSmoothed[1]*3, fftSmoothed[1]*3);
        // draw to screen
        glBegin(GL_QUADS);  
            glTexCoord2f(0, 0);				glVertex2f(0, 0);  
            glTexCoord2f(WIDTH, 0);			glVertex2f(WIDTH, 0);  
            glTexCoord2f(WIDTH, HEIGHT);	glVertex2f(WIDTH, HEIGHT); 
            glTexCoord2f(0, HEIGHT);		glVertex2f(0, HEIGHT);   
        glEnd();      
    shader.end();  
        
    // draw the fft results:
	ofTranslate(WIDTH/2, HEIGHT/2);
	int r=255,g=137,b=137;
	bool rp=true, gp=false, bp=true;
    float width = (float)(5*118) / (nBandsToGet-12);
    for (int i = 0;i < 118; i++){
		
		ofPushMatrix();
		ofRotate(-count*2);
		ofRotate(-3.05*i);
		if(i>19) rp = false;
		if(i>38) gp = true;
		if(i>57) bp = false;
		if(i>76) rp = true;
		if(i>95) gp = false;
		color_shift(r,g,b,rp,gp,bp);	
		ofSetColor(r,g,b);	
		ofRect(0,120,width,-(fftSmoothed[i] * (70+8*i)));
		ofCircle(0,120+fftSmoothed[i]*(60+4*i), 1+fftSmoothed[i]*4);
			
		ofPopMatrix(); 
    }
	/*if(fftSmoothed[27]>0.05f){
		for(int i=0; i<10; i++){
			int rand = ofRandom(5,10);
			proj.push_back(
				Particle(ofVec2f(0+cos(36*i)*90, 90-sin(36*i)*90), ofVec2f(rand*cos(36*i), rand*sin(36*i)), 255, 255, 50));		
		}
	}*/

	for (vector<Particle>::iterator i = proj.begin(); i!=proj.end();) {
		if(i->death()) {
			i = proj.erase(i);
		}
		else{
			i->applyForces();
			i->update();
			i++;
		}
	}
	
	for (int i=0; i<proj.size(); i++) {
		proj[i].draw();
	}

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    int max = ofRandom(10,20);
	for(int i=0; i<max; i++) {
		proj.push_back(Particle(
			ofVec2f(x-WIDTH/2,y-HEIGHT/2), ofVec2f(ofRandom(-20,20), ofRandom(-20,20)), 255, 255, 50));
	}

}
 
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    int max = ofRandom(10,20);
	for(int i=0; i<max; i++) {
		proj.push_back(Particle(
			ofVec2f(x-WIDTH/2,y-HEIGHT/2), ofVec2f(ofRandom(-20,20), ofRandom(-20,20)), 255, 255, 50));
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

