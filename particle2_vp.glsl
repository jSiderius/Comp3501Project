#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.0; // Gravity
float speed = 4.0; // Controls the speed of the explosion
float upward = 1.0; // additional y velocity for all particles

void main()
{   
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    float advanced_timer = timer + 1000.0f;
    float interval = abs(norm.x * norm.y * norm.z * 1000);
    float circtime = advanced_timer - interval * floor(advanced_timer / interval);
    float t = circtime; // Our time parameter

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += norm.x*t*speed/3 - grav*speed*up_vec.x*t*t;
	
	// add "upward" to y speed to launch the particles vertically -- can easily omit
    position.y += (upward+norm.y)*speed*speed/t - grav*speed*up_vec.y*t*t + 2;
    position.z += norm.z*t*speed/3 - grav*speed*up_vec.z*t*t;
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define color of vertex
    vertex_color = vec3(1.0, 1-(t/4), 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}

