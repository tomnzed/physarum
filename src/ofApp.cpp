#include "ofApp.h"

#include <cstdlib>
#include <ctime>
#include <chrono>
#include <math.h>

const float POPULATION = 0.5;
const float DECAY_FACTOR = 0.15;
const float SENSE_ANGLE = 15;
const size_t SENSE_OFFSET = 3;
const float ROTATE_ANGLE = 30;
const float STEP_SIZE = 1;
const size_t CHEMO_DEPOSIT = 1;
const size_t MAX_CHEMO = 5;

const float DISPLAY_NORMAL_X = 0.4;
const float DISPLAY_NORMAL_Y = 0.6;


std::ofstream log_error(const char* file, size_t line, const std::string & message )
{
    std::stringstream ss;
    ss << file << ":" << line << " - " << message;
    ofLogError( "ofApp", ss.str() );
}

#define LOG_ERROR(x) log_error(__FILE__,__LINE__,x)

const double ofApp::RenderTimes:: UPDATE_RATE = 0.01;

std::ostream& operator<<( std::ostream& os, const ofApp::RenderTimes& times )
{
    return os
        << "Agent vbo = " << times.agent_vbo.duration / std::chrono::milliseconds( 1 ) << "ms, "
        << "current sense = " << times.draw_current_sense.duration / std::chrono::milliseconds( 1 ) << "ms, "
        << "update agents = " << times.update_agents.duration / std::chrono::milliseconds( 1 ) << "ms, "
        << "draw = " << times.draw_to_screen.duration / std::chrono::milliseconds( 1 ) << "ms";
}

//--------------------------------------------------------------
void ofApp::setup(){
    
    srand(20);
    
    
    int err = glGetError();
    if (err != GL_NO_ERROR) {
        LOG_ERROR( ofToString(gluErrorString(err)));
    }
    
    ofBackgroundHex(0x57554c);
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    diffuse_shader.load("shader/diffuse");
    agent_update_shader.load("shader/agent_update");
    point_shader.load("shader/point");
    display_shader.load("shader/display");
    
    screen_size = { 1280, 800 };
    
    ofSetWindowShape( screen_size.x, screen_size.y );
    
    ofFboSettings fbo_settings;
    fbo_settings.width = screen_size.x;
    fbo_settings.height = screen_size.y;
    fbo_settings.internalformat = GL_RGBA32F;
    
    err = glGetError();
    if (err != GL_NO_ERROR) {
        LOG_ERROR( ofToString(gluErrorString(err)));
    }
    
    agent_fbo.allocate(fbo_settings);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        LOG_ERROR( ofToString(gluErrorString(err)));
    }
    sense_fbo.allocate(fbo_settings);
    err = glGetError();
    if (err != GL_NO_ERROR) {
        LOG_ERROR( ofToString(gluErrorString(err)));
    }
    last_sense_fbo.allocate(fbo_settings);
    
    err = glGetError();
    if (err != GL_NO_ERROR) {
        LOG_ERROR( ofToString(gluErrorString(err)));
    }
    
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
            auto angle = static_cast<float>( std::rand() ) / RAND_MAX * M_PI * 2;
            auto r = static_cast<float>( std::rand() ) / RAND_MAX;
            
            auto x = sin( angle ) * r * 0.25 + 0.5;
            auto y = cos( angle ) * r * 0.25 + 0.5;
//            auto heading = angle / ( M_PI * 2 );
//            auto x = static_cast<float>( std::rand() ) / RAND_MAX;
//            auto y = static_cast<float>( std::rand() ) / RAND_MAX;
            auto heading = static_cast<float>( std::rand() ) / RAND_MAX;
            agents[ i ].x = x;
            agents[ i ].y = y;
            agents[ i ].heading = heading;
            
//            std::cout << "Created point at " << x << ", " << y << " with heading " << heading << std::endl;
//            std::cout << x << ", " << y << ", " << heading << std::endl;
        }
        
        agent_vbo.setVertexData( &agents[0].x, 2, count, GL_DYNAMIC_DRAW, sizeof(Agent) );
        update_fbo.allocate( count / update_fbo_width, update_fbo_width, GL_RGBA32F );
        update_pbo.allocate( count / update_fbo_width, update_fbo_width, GL_RGBA, GL_DYNAMIC_READ );
    }
    
    const GLubyte* renderer = glGetString(GL_RENDERER);
    
    std::cout << "Using renderer " << renderer << std::endl;
}

//--------------------------------------------------------------
void ofApp::update()
{

    if( mSaveNextFrame )
    {
        mSaveNextFrame = false;
        
        int w = sense_fbo.getWidth();
        int h = sense_fbo.getHeight();
        ofPixels pixels;
        pixels.allocate( w, h, 4 );
        sense_fbo.readToPixels( pixels );
        
        ofImage image;
        image.allocate( w, h, ofImageType::OF_IMAGE_COLOR_ALPHA );
        image.setFromPixels( pixels );
        
        const std::string filename{ "sense.png" };
        image.save( filename );
        
        std::cout << "Saved image " << filename;
    }
    
}

void ofApp::DrawPrettySense()
{
    auto timer = mRenderTimes.draw_to_screen.TimeThisScope();
    
    display_shader.begin();
    display_shader.setUniformTexture("senseTexture", sense_fbo.getTexture(), 2);
    display_shader.setUniform1f( "normalX", DISPLAY_NORMAL_X );
    display_shader.setUniform1f( "normalY", DISPLAY_NORMAL_Y );
    
    {
        ofRectangle rect ( 0, 0, screen_size.x, screen_size.y );
        ofDrawRectangle( rect );
    }
    
    display_shader.end();
}

void ofApp::DrawAgents()
{
    auto timer = mRenderTimes.agent_vbo.TimeThisScope();
    
    agent_vbo.setVertexData( &agents[0].x, 2, count, GL_DYNAMIC_DRAW, sizeof(Agent) );
    ofClear(0, 0, 0);
    
    ofSetColor(255);
    
    point_shader.begin();
    point_shader.setUniform2fv( "screenSize", screen_size.getPtr() );
    
    glPointSize( 1 );
    agent_vbo.draw( GL_POINTS, 0, agents.size() );
    point_shader.end();
}

void ofApp::DrawSense()
{
    auto timer = mRenderTimes.draw_current_sense.TimeThisScope();
    
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
}

void ofApp::UpdateAgentPositions()
{
    auto timer = mRenderTimes.update_agents.TimeThisScope();
    
    update_fbo.begin();
    
    // @todo: Set this from the PBO so to not require the data on the CPU.
    agent_texture.loadData( &agents[0].x, count / update_fbo_width, update_fbo_width, GL_RGBA );
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
    
    update_pbo.readPixels( update_fbo );
    auto gpu_memory = update_pbo.map( GL_PIXEL_PACK_BUFFER, GL_READ_ONLY );
    std::copy( gpu_memory, gpu_memory + agents.size() * sizeof(Agent) / sizeof(float), &agents[0].x );
    update_pbo.unmap( GL_PIXEL_PACK_BUFFER );
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    if( !mRunning )
    {
        DrawPrettySense();
        return;
    }
    
    //----------------------------------------------------------
    // Draw agents
    agent_fbo.begin();
    DrawAgents();
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
    DrawSense();
    sense_fbo.end();
    
    //----------------------------------------------------------
    // Update the agent positions
    UpdateAgentPositions();
    
    //----------------------------------------------------------
    // Draw to screen
    DrawPrettySense();
    
    auto as_seconds = []( std::chrono::steady_clock::time_point t )
    {
        return std::chrono::duration_cast<std::chrono::seconds>( t.time_since_epoch() ).count();
    };
    
    if( as_seconds( mLastFrameTime ) != as_seconds( std::chrono::steady_clock::now() ) )
    {
        std::cout << mRenderTimes << std::endl;
    }
    
    mLastFrameTime = std::chrono::steady_clock::now();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch( key )
    {
        case ' ':
            mRunning = !mRunning;
            break;
            
        case 'c':
            mSaveNextFrame = true;
            break;
    }
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
