#version 400

// Definition of the geometry shader
layout (points) in;
layout (triangle_strip, max_vertices = 66) out;

// Attributes passed from the vertex shader
in vec3 vertex_color[];
in float timestep[];

// Uniform (global) buffer
uniform mat4 projection_mat;

// Simulation parameters (constants)
uniform float particle_size = 0.01;

// Attributes passed to the fragment shader
out vec4 frag_color;
out vec2 tex_coord;

void main(void) {
    // Get the position of the particle
    vec4 position = gl_in[0].gl_Position;

    // Define the number of segments for the circle
    const int segments = 32;
    float angleIncrement = (2.0 * 3.14159265359) / float(segments);

    // Define the radius of the circle
    float radius = particle_size;

    // Emit vertices around the circumference to create a filled circle
    for (int i = 0; i <= segments; i++) {
        float angle = float(i) * angleIncrement;
        vec4 v = vec4(position.x + radius * cos(angle), position.y + radius * sin(angle), position.z, 1.0);

        gl_Position = projection_mat * position;
        frag_color = vec4(vertex_color[0], 1.0);
        EmitVertex();

        gl_Position = projection_mat * v;
        frag_color = vec4(vertex_color[0], 1.0);
        tex_coord = vec2(0.5, 0.5);
        EmitVertex();
    }

    EndPrimitive();
}

