#version 400

// Definition of the geometry shader
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

// Attributes passed from the vertex shader
in vec3 vertex_color[];
in float timestep[];

// Uniform (global) buffer
uniform mat4 projection_mat;

// Simulation parameters (constants)
uniform float particle_size = 0.01;

// Attributes passed to the fragment shader
out vec4 frag_color;

void main(void) {
    // Get the position of the particle
    vec4 position = gl_in[0].gl_Position;

    // Define particle size
    float p_size = particle_size * 5;

    // Define the number of particles in the circle
    int numParticles = 8;

    // Calculate positions in a circle
    for (int i = 0; i < numParticles; i++) {
        float angle = float(i) * (6.28 / float(numParticles)); // Angle in radians
        vec4 offset = vec4(cos(angle) * p_size, sin(angle) * p_size, 0.0, 0.0);
        vec4 v = position + offset;

        // Transform to clip space
        gl_Position = projection_mat * v;

        // Pass color to fragment shader
        frag_color = vec4(vertex_color[0], 1.0);

        // Emit vertex
        EmitVertex();
    }

    EndPrimitive();
}
 