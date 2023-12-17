#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 tex_coord;  // Add this line

// Uniform (global) buffer
uniform sampler2D texture_map;

uniform vec3 object_color = vec3(0.6, 0.2, 0.01);

void main (void)
{
    // Get pixel from texture using the tex_coord
    vec4 outval = texture(texture_map, tex_coord);

    // Adjust specified object color according to the grayscale texture value
    // outval = vec4(outval.r * object_color.r, outval.g * object_color.g, outval.b * object_color.b, sqrt(sqrt(outval.r)) * frag_color.a);

    // Adjust specified object color
    // outval.rgb *= object_color.rgb;

    // Set output fragment color
    gl_FragColor = outval;
}

