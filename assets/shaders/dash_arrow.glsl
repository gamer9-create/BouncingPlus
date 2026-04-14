#version 300 es

precision mediump float;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;


// NOTE: Add here your custom variables

void main()
{
    float dx = 0.1;
    float dy = 0.1;

    vec2 coord = vec2(dx*round(fragTexCoord.x/dx), dy*round(fragTexCoord.y/dy));
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, coord + vec2(0.0, time * 2.25));

    vec4 second_to_final_color = texelColor*colDiffuse*fragColor;
    finalColor = vec4(second_to_final_color.rgb, second_to_final_color.a * (1.0-(abs((coord.y/6.0)-0.5)*2.0)));

}

