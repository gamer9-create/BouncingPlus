#version 130

precision mediump float;

//	<https://www.shadertoy.com/view/4dS3Wd>
//	By Morgan McGuire @morgan3d, http://graphicscodex.com
//
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
    float i = floor(x);
    float f = fract(x);
    float u = f * f * (3.0 - 2.0 * f);
    return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);

    // Four corners in 2D of a tile
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    // Simple 2D Lerp using smoothstep envelope between the values.
    // return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
    //			mix(c, d, smoothstep(0.0, 1.0, f.x)),
    //			smoothstep(0.0, 1.0, f.y)));

    // Same code, with the clamps in smoothstep and common subexpressions
    // optimized away.
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

uniform int renderWidth = 800;
uniform int renderHeight = 450;
uniform float time = 0;

void main()
{
    vec2 coord = fragTexCoord;

    float num = 3 + (sin((time + coord.x * 10 + coord.y * 10) * 3)*2.5);

    coord += vec2(sin(time) * (num / renderWidth) / 2.5, cos(time) * (num / renderHeight) / 2.5);

    bool edge = (texture2D(texture0, coord - vec2(num / renderWidth, 0)).a == 0 || texture2D(texture0, coord + vec2(num / renderWidth, 0)).a == 0 ||
    texture2D(texture0, coord - vec2(0, num / renderHeight)).a == 0 || texture2D(texture0, coord + vec2(0, num / renderHeight)).a == 0
    || texture2D(texture0, coord + vec2(num / renderWidth, num / renderHeight)).a == 0
    || texture2D(texture0, coord + vec2(num / renderWidth, -num / renderHeight)).a == 0
    || texture2D(texture0, coord + vec2(-num / renderWidth, num / renderHeight)).a == 0
    || texture2D(texture0, coord + vec2(-num / renderWidth, -num / renderHeight)).a == 0);

    edge = edge && texture2D(texture0,coord).a != 0;

    if (!edge) {
        if ((int(coord.x * renderWidth) & 1) == 0 && (int(coord.y * renderHeight) & 1) == 0) {
            gl_FragColor = texture2D(texture0,coord)*vec4(0.1, 1, 0.1, min(max(noise((coord * 50) + vec2(time / 100, time / 100))*1.5, 0.1), 1.0));
        }
    } else {
        gl_FragColor = vec4(0.1,1,0.1,1);
    }

}
