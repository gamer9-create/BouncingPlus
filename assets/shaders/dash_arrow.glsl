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
    // Texel color fetching from texture sampler
    vec4 texelColor = texture2D(texture0, fragTexCoord + vec2(0, time * 2.25));

    // NOTE: Implement here your fragment shader code

    // final color is the color from the texture
    //    times the tint color (colDiffuse)
    //    times the fragment color (interpolated vertex color)
    //1-(abs((fragTexCoord.y/6)-0.5)*2)
    vec4 second_to_final_color = texelColor*colDiffuse*fragColor;
    finalColor = vec4(second_to_final_color.rgb, second_to_final_color.a * (1-(abs((fragTexCoord.y/6)-0.5)*2)));
}

