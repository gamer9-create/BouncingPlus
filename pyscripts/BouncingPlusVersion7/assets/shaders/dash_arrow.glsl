#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

void main()
{
    float dx = 8 * (1.0 / 80);
    float dy = 8 * (1.0 / 80);

    vec2 coord = vec2(dx*round(fragTexCoord.x/dx), dy*round(fragTexCoord.y/dy));
    // Texel color fetching from texture sampler
    vec4 texelColor = texture2D(texture0, coord + vec2(0, time * 2.25));

    vec4 second_to_final_color = texelColor*colDiffuse*fragColor;
    finalColor = vec4(second_to_final_color.rgb, second_to_final_color.a * (1-(abs((coord.y/6)-0.5)*2)));

}

