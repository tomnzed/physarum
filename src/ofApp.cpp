#include "ofApp.h"

#include <cstdlib>
#include <math.h>

const float POPULATION = 1.0;
const float DECAY_FACTOR = 0.2;
const float SENSE_ANGLE = 25;
const size_t SENSE_OFFSET = 6;
const float ROTATE_ANGLE = 40;
const float STEP_SIZE = 1;
const size_t CHEMO_DEPOSIT = 1;
const size_t MAX_CHEMO = 2;


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
    display_shader.load("shader/display");
    
    screen_size = { 640, 400 };
    
    ofSetWindowShape( screen_size.x, screen_size.y );
    
    ofFboSettings fbo_settings;
    fbo_settings.width = screen_size.x;
    fbo_settings.height = screen_size.y;
    fbo_settings.internalformat = GL_RGBA32F;
    fbo_settings.wrapModeVertical = GL_REPEAT;
    fbo_settings.wrapModeHorizontal = GL_REPEAT;
    
    agent_fbo.allocate(fbo_settings);
    sense_fbo.allocate(fbo_settings);
    last_sense_fbo.allocate(fbo_settings);
    
    // Clear the sense fbo
    sense_fbo.begin();
    ofClear(0, 0, 0);
    sense_fbo.end();
    
    {
        const size_t w = screen_size.x;
        const size_t h = screen_size.y;
        
        const size_t pixels = h * w;
        update_fbo_width = 1000;
        count = pixels * POPULATION;
        count = std::floor( static_cast<float>( count ) / update_fbo_width ) * update_fbo_width;  // round it so that there is an agent per texel.
        
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
//            std::cout << x << ", " << y << ", " << heading << std::endl;
        }
        agent_vbo.setVertexData( &agents[0].x, 2, count, GL_DYNAMIC_DRAW, sizeof(Agent) );
        update_fbo.allocate( count / update_fbo_width, update_fbo_width, GL_RGBA32F );
    }
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
    diffuse_shader.setUniform1f( "maxChemoAttract", MAX_CHEMO );  // @todo Increase this and add a final shader to display the likely dim fbo
    diffuse_shader.setUniform1f( "depositChemoAttract", CHEMO_DEPOSIT );
    diffuse_shader.setUniform1f( "chemoAttractDecayFactor", DECAY_FACTOR );

    {
        ofRectangle rect ( 0, 0, screen_size.x, screen_size.y );
        ofDrawRectangle( rect );
    }

    diffuse_shader.end();

    sense_fbo.end();
    
    
    //----------------------------------------------------------
    // Update the agent positions
    // @todo: Improve the storage precision of agent positions in this texture
    agent_texture.loadData( &agents[0].x, count / update_fbo_width, update_fbo_width, GL_RGBA );

    update_fbo.begin();

    agent_update_shader.begin();
    agent_update_shader.setUniformTexture("agentTexture", agent_texture, 1);
    agent_update_shader.setUniformTexture("senseTexture", last_sense_fbo.getTexture(), 2);
    
    agent_update_shader.setUniform1f( "senseAngle", SENSE_ANGLE );
    agent_update_shader.setUniform1f( "rotateAngle", ROTATE_ANGLE );
    agent_update_shader.setUniform1f( "senseOffset", SENSE_OFFSET );
    agent_update_shader.setUniform1f( "stepSize", STEP_SIZE );
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
    
    display_shader.begin();
    display_shader.setUniformTexture("senseTexture", sense_fbo.getTexture(), 2);
    display_shader.setUniform1f( "maxChemoAttract", MAX_CHEMO );
    display_shader.setUniform1f( "depositChemoAttract", CHEMO_DEPOSIT );
    
    {
        ofRectangle rect ( 0, 0, screen_size.x, screen_size.y );
        ofDrawRectangle( rect );
    }
    
    display_shader.end();
    
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
