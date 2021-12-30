#include "ofApp.h"

#include <cstdlib>
#include <math.h>

// R - position X
// G - position Y
// B - heading
// A - state
ofTexture create_initial_agents( const size_t w, const size_t h, const float population = 0.6 )
{
    ofTexture texture;
    
    std::vector<float> data;
    data.resize( w * h * 4, 0.0 );
    
    const size_t pixels = h * w;
    
    for( size_t i = 0; i < pixels * population; ++i )
    {
        data[ i * 4 ]     = static_cast<float>( std::rand() ) / RAND_MAX * w;
        data[ i * 4 + 1 ] = static_cast<float>( std::rand() ) / RAND_MAX * h;
        data[ i * 4 + 2 ] = static_cast<float>( std::rand() ) / RAND_MAX;
        data[ i * 4 + 3 ] = 1.0;  // alive
    }
    
    texture.loadData( data.data(), w, h, GL_RGBA );
    return texture;
}


//--------------------------------------------------------------
void ofApp::setup(){
    
    srand(20);
    
    
    ofBackgroundHex(0x57554c);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    diffuse_shader.load("shader/diffuse");
    agent_update_shader.load("shader/agent_update");
    point_shader.load("shader/point");
    
    screen_size = { 1280, 800 };
    
    ofSetWindowShape( screen_size.x, screen_size.y );
    
    agent_fbo.allocate(screen_size.x, screen_size.y, GL_RGBA32F);
    sense_fbo.allocate(screen_size.x, screen_size.y, GL_RGBA32F);
    last_sense_fbo.allocate(screen_size.x, screen_size.y, GL_RGBA32F);
    
    
    // Clear the sense fbo
    sense_fbo.begin();
    ofClear(0, 0, 0);
    sense_fbo.end();
    
    {
        const size_t w = screen_size.x;
        const size_t h = screen_size.y;
        const float population = 0.15;
        
        const size_t pixels = h * w;
        count = 100;
//        count = pixels * population;
        
        agents.resize( count, {} );
        
        for( size_t i = 0; i < count; ++i )
        {
            auto x = static_cast<float>( std::rand() ) / RAND_MAX;
            auto y = static_cast<float>( std::rand() ) / RAND_MAX;
            auto heading = static_cast<float>( std::rand() ) / RAND_MAX;
            agents[ i ].x = x;
            agents[ i ].y = y;
            agents[ i ].heading = heading;
            
            points.push_back(glm::vec2(x, y));
            
//            std::cout << "Created point at " << x << ", " << y << " with heading " << heading << std::endl;
            std::cout << x << ", " << y << ", " << heading << std::endl;
        }
        agent_vbo.setVertexData( &agents[0].x, 2, count, GL_DYNAMIC_DRAW, sizeof(Agent) );
        update_fbo.allocate( count, 1, GL_RGBA32F );
    }
    
    image.load("img.jpg");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //----------------------------------------------------------
    // Draw agent
    //
    agent_vbo.setVertexData( &agents[0].x, 2, count, GL_DYNAMIC_DRAW, sizeof(Agent) );
    agent_fbo.begin();
    ofClear(0, 0, 0);
    
    ofSetColor(255);
    
    // @todo figure out how to map these from norm coords to abs and draw properly :)
    point_shader.begin();
    point_shader.setUniform2fv( "screenSize", screen_size.getPtr() );
    
    glPointSize( 1 );
    agent_vbo.draw( GL_POINTS, 0, points.size() );
    point_shader.end();
    
    agent_fbo.end();
    
    //----------------------------------------------------------
    // Store the last sense
    last_sense_fbo.begin();
    ofClear(0, 0, 0);
    sense_fbo.draw(0, 0);
    last_sense_fbo.end();
    
    //----------------------------------------------------------
    // Create the current sense
    sense_fbo.begin();
    ofClear(0, 0, 0);

    diffuse_shader.begin();
    diffuse_shader.setUniformTexture("agentTexture", agent_fbo.getTexture(), 1);
    diffuse_shader.setUniformTexture("senseTexture", last_sense_fbo.getTexture(), 2);
    diffuse_shader.setUniform2fv( "screenSize", screen_size.getPtr() );

    {
        ofRectangle rect ( 0, 0, screen_size.x, screen_size.y );
        ofDrawRectangle( rect );
    }

    diffuse_shader.end();

    sense_fbo.end();
    
    
    //----------------------------------------------------------
    // Update the agent positions
    // @todo: Improve the storage precision of agent positions in this texture
    agent_texture.loadData( &agents[0].x, count, 1, GL_RGBA );

    update_fbo.begin();

    agent_update_shader.begin();
    agent_update_shader.setUniformTexture("agentTexture", agent_texture, 1);
    agent_update_shader.setUniformTexture("senseTexture", last_sense_fbo.getTexture(), 2);
    
    agent_update_shader.setUniform1f( "senseAngle", 22.5 );
    agent_update_shader.setUniform1f( "rotateAngle", 45. );
    agent_update_shader.setUniform1f( "senseOffset", 9. );
    agent_update_shader.setUniform1f( "stepSize", 0.1 );
    agent_update_shader.setUniform2fv( "screenSize", screen_size.getPtr() );
    
    {
        ofRectangle rect ( 0, 0, update_fbo.getWidth(), update_fbo.getHeight() );
        ofDrawRectangle( rect );
    }
    agent_update_shader.end();
    update_fbo.end();

    ofPixels_<float> pixels;
    update_fbo.getTexture().readToPixels( pixels );
    std::copy( pixels.getData(), pixels.getData() + pixels.size(), &agents[0].x );

    //----------------------------------------------------------
    
//    agent_fbo.draw(0, 0);
    sense_fbo.draw( 0, 0 );
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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
