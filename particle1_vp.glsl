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
float speed = 0.1; // Controls the speed of the explosion
float upward = 0.0; // additional y velocity for all particles

void main()
{
        
    // Let time cycle every four seconds
    float circtime = timer - 8.0 * floor(timer / 8);
    float t = circtime; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    //ORIG
    // // Move point along normal and down with t*t (acceleration under gravity)
    // position.x += norm.x*4*speed - grav*speed*up_vec.x*t*t;
	
	// // add "upward" to y speed to launch the particles vertically -- can easily omit
    // position.y += (upward+norm.y)*4*speed - grav*speed*up_vec.y*t*t;

    // position.z += norm.z*4*speed - grav*speed*up_vec.z*t*t;
    //ORIG END


    float randx = 0;
    float randy = 0;
    float randz = 0;
    // SPREAD --> norm.x*4*speed + SWARM PATH --> sin(timer) * cos(timer * 2) * 2.0 + color.x INDIVIDUAL PATH --> ;
    // randx = norm.x*4*speed +  sin(timer) * cos(timer * 2) * 2.0 + color.x;
    // randy = norm.y*4*speed +  sin(timer * 0.5) * sin(timer * 1.7) * 0.5 + color.y;
    // randz = norm.z*4*speed +  cos(timer  + 1.1) + 2.0 + color.z; 

    randx = norm.x*20*speed + sin(color.x * timer * 1.6);
    randy = norm.y*20*speed + sin(color.y * timer * 1.3);
    randz = norm.z*20*speed + sin(color.z * timer * 0.8);

    position.x = randx;
    position.y = randy;
    position.z = randz;
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    // vertex_color = color.rgb; // Color defined during the construction of the particles
    //vertex_color = object_color; // Uniform color 
    // vertex_color = vec3(t, 0.0, 1-t); // red-purple dynamic color
    vertex_color = vec3(1.0, 1-t + norm.x + norm.y + norm.z, 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}
