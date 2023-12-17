#version 140

// Attributes passed from the vertex shader
in vec3 position_interp; //Passed position
in vec3 normal_interp; //Passed normal 
in vec4 color_interp; //Passed color value for the pixel
in vec2 uv_interp;
uniform vec3 light_pos; 		//UNIFORM FOR LIGHT POSITION
uniform vec3 view_pos; 		//UNIFORM FOR PLAYER POSITION
uniform vec4 light_color; 	//UNIFORM FOR LIGHT COLOR
uniform vec4 ambient_color; //UNIFORM FOR OBJECT COLOR
uniform float spec_power; 	//UNIFORM FOR SPECULAR POWER

// Uniform (global) buffer
uniform sampler2D texture_map;

void main() 
{
	vec2 uv_use = 2*uv_interp;
    vec4 pixel = texture(texture_map, uv_use);

	vec3 view_dir = normalize(view_pos - position_interp);
	vec3 light_dir = normalize(light_pos - position_interp); // light direction, object position as origin
	vec3 normal = normalize(normal_interp); // must normalize interpolated normal
	vec3 halfway = normalize((view_dir+light_dir)/2); // halfway vector -- note /2 pointless, just there for clarity

	//DIFFUSE LIGHTING 
	float diffuse = max(0.0, dot(normal,light_dir)); 
	
	//SPECULAR LIGHTING implementation uses blinn-Phong
	float spec = max(0.0,dot(normal,halfway)); // cannot be negative 
	spec = pow(spec,spec_power); // specular power

	//AMBIENT LIGHTING 
	float amb = 0.2; //Float represents the ambient strength the Ambient color / Object color is passed in the ambient_color variable as per the assignment 
	
	// spec = 0;  // turn off specular
	// diffuse = 0; // turn off diffuse
    // amb = 0.0; // turn off ambient

    // Use variable "pixel", surface color, to help determine fragment color
    gl_FragColor = light_color*pixel*diffuse +
	   light_color*vec4(1,1,1,1)*spec + // specular might not be colored
	   light_color*pixel*amb*ambient_color; // ambcol not used, could be included here
}

