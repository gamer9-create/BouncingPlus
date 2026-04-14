#version 130

precision mediump float;

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;


// NOTE: Add here your custom variables

void main()
{
    float dx = 8 * (1.0 / 80);
    float dy = 8 * (1.0 / 80);

    vec2 coord = vec2(dx*round(fragTexCoord.x/dx), dy*round(fragTexCoord.y/dy));
    // Texel color fetching from texture sampler
    vec4 texelColor = texture2D(texture0, coord + vec2(0, time * 2.25));

    vec4 second_to_final_color = texelColor*colDiffuse*fragColor;
    gl_FragColor = vec4(second_to_final_color.rgb, second_to_final_color.a * (1-(abs((coord.y/6)-0.5)*2)));

}

