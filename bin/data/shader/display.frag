#version 150

uniform sampler2DRect senseTexture;
uniform float normalX;
uniform float normalY;

uniform int downsample;
uniform vec2 screenSize;

in vec2 texCoordVarying;
in vec4 vertColour;
out vec4 outputColor;

void main()
{
    vec4 chemo;

    if( downsample == 1 )
    {
        chemo = texture( senseTexture, gl_FragCoord.st);
    }
    else if( downsample == 2 )
    {
        vec2 c = gl_FragCoord.st * 2;
        vec2 i = 1.0 / screenSize / 2;
        chemo = texture( senseTexture, c + i * vec2( -1, -1 ) ) * 0.25 + 
                texture( senseTexture, c + i * vec2( -1,  1 ) ) * 0.25 + 
                texture( senseTexture, c + i * vec2(  1, -1 ) ) * 0.25 + 
                texture( senseTexture, c + i * vec2(  1,  1 ) ) * 0.25;
    }
    else if( downsample == 4 )
    {
        chemo = texture( senseTexture, gl_FragCoord.st);
    }

    if( normalX != normalY )
    {
        // This seems to just make everything display as greyer if maxChemoAttract is much bigger than 1 or 2 x depositChemoAttract. 
        if( chemo.r > normalX )
        {
            // chemo.rgb = vec3(1.);
            chemo.rgb = ( chemo.rgb - normalX ) / (1. - normalX) * ( 1. - normalY ) + normalY;
        }
        else
        {
            chemo.rgb = chemo.rgb * normalY / normalX;
        }
    }
    

    outputColor = vec4( 1. - chemo.rgb, 1. );
}
