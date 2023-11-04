#version 140

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
uniform vec3 light_pos; 		//UNIFORM FOR LIGHT POSITION
uniform vec4 light_color; 	//UNIFORM FOR LIGHT COLOR
uniform vec4 ambient_color; //UNIFORM FOR OBJECT COLOR
uniform float spec_power; 	//UNIFORM FOR SPECULAR POWER

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform float timer; 

void main() 
{		
    float rectangleHeight = 0.04; 
    float rectangleWidth = 0.02; 

    vec4 pixel; 

    int row_index = int((uv_interp.x - mod(uv_interp.x, rectangleWidth)) / rectangleWidth);
    int col_index = int((uv_interp.y - mod(uv_interp.y, rectangleHeight)) / rectangleHeight);

    float adj_sec = 10;
    float red_adjust; 
    if(mod(timer,2*adj_sec) < adj_sec)
        red_adjust = mod(timer,adj_sec) / adj_sec;
    else
        red_adjust = 1.0 - mod(timer,adj_sec) / adj_sec;

    if ((row_index % 2 == 0 && col_index % 2 == 0) || (row_index % 2 != 0 && col_index % 2 != 0))
        pixel = vec4(0,1,0,1);
	else
		pixel = vec4(red_adjust,0.1,0.1,1);

	vec3 view_pos = vec3(0,0,0); // view position -- 0 in view space

	vec3 view_dir = normalize(view_pos - position_interp);
	vec3 light_dir = normalize(light_pos - position_interp); // light direction, object position as origin
	vec3 normal = normalize(normal_interp); // must normalize interpolated normal
	
	//DIFFUSE LIGHTING 
	float diffuse = max(0.0, dot(normal,light_dir)); 
	
	//SPECULAR LIGHTING implementation uses Phong saging by not using a halfway vector as per the assignment 
	float spec = max(0.0,dot(normal,light_dir)); // cannot be negative 
	spec = pow(spec,spec_power); // specular power

	//AMBIENT LIGHTING 
	float amb = 0.4; //Float represents the ambient strength the Ambient color / Object color is passed in the ambient_color variable as per the assignment 
	
	
	// spec = 0;  // turn off specular
	// diffuse = 0; // turn off diffuse
    // amb = 0.0; // turn off ambient

    // Use variable "pixel", surface color, to help determine fragment color
    gl_FragColor = light_color*pixel*diffuse +
	   light_color*vec4(1,1,1,1)*spec + // specular might not be colored
	   light_color*pixel*amb*ambient_color; // ambcol not used, could be included here
}



