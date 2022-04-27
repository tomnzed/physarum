#version 150

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
uniform mat4 textureMatrix;

in vec4 position;
in vec2 texcoord;
in vec4 normal;
in vec4 color;

out vec2 texCoordVarying;

void main()
{
    #ifdef INTEL_CARD
    color = vec4(1.0); // for intel HD cards
    normal = vec4(1.0); // for intel HD cards
    #endif

    texCoordVarying = texcoord;

    // @todo Get the proper position and scaling for this, as this doesn't make much sense
    gl_Position = ( position - vec4( 0.5, 0.5, 0, 0 ) ) * vec4( 2., 2., 1., 1. );
}
