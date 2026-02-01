#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables

// NOTE: Render size values must be passed from code
uniform int renderWidth = 800;
uniform int renderHeight = 450;

float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main()
{
    // Texel color fetching from texture sampler
    vec4 origin = texture(texture0, fragTexCoord);
    vec4 texelColor = origin * weight[0];

    for (int i = 1; i < 3; i++)
    {
        texelColor += texture(texture0, fragTexCoord + vec2(offset[i] / float(renderWidth), offset[i] / float(renderHeight)), 0.0)*weight[i];
        texelColor += texture(texture0, fragTexCoord + vec2(-offset[i] / float(renderWidth), offset[i] / float(renderHeight)), 0.0)*weight[i];
        texelColor += texture(texture0, fragTexCoord + vec2(offset[i] / float(renderWidth), -offset[i] / float(renderHeight)), 0.0)*weight[i];
        texelColor += texture(texture0, fragTexCoord + vec2(-offset[i] / float(renderWidth), -offset[i] / float(renderHeight)), 0.0)*weight[i];

        texelColor += texture(texture0, fragTexCoord + vec2(offset[i] / float(renderWidth), 0), 0.0)*weight[i];
        texelColor += texture(texture0, fragTexCoord + vec2(-offset[i] / float(renderWidth), 0), 0.0)*weight[i];
        texelColor += texture(texture0, fragTexCoord + vec2(0, offset[i] / float(renderHeight)), 0.0)*weight[i];
        texelColor += texture(texture0, fragTexCoord + vec2(0, -offset[i] / float(renderHeight)), 0.0)*weight[i];
    }

    finalColor = texelColor / 4.0;
}