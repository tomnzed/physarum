#version 150

uniform sampler2DRect senseTexture;
uniform float blurAmnt;
uniform float maxChemoAttract;
uniform float depositChemoAttract;

in vec2 texCoordVarying;
in vec4 vertColour;
out vec4 outputColor;

void main()
{
    vec4 chemo = texture( senseTexture, gl_FragCoord.st);


    if( maxChemoAttract != depositChemoAttract )
    {
        float normalX = depositChemoAttract / maxChemoAttract;

        float normalY = 0.6;

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
