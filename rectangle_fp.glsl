#version 140

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform float timer; 

void main() 
{
    // Remove this code and replace with your own.

	float bar = 0.02;
	float rh = 0.1;// / (4 + 0*abs(uv_interp.x - 0.5));
	float rw = 0.04;// / (1 + 3*abs(uv_interp.y - 0.5));
	vec4 outcol = vec4(0,1,0,1);
	vec2 uv_use = uv_interp+0.04*vec2(0,0.5+0.5*sin(uv_interp.x*20));
	//uv_use.x = abs(uv_use.x-0.5);
	//uv_use.y = abs(uv_use.y-0.5);
//	uv_use.x = 2*uv_use.x*(uv_use.x+0.4);
//	uv_use.y = uv_use.y*(uv_use.y+0.2)+0.4;

	float widd = mod(uv_use.x, (bar+rw) );
	float heii = mod(uv_use.y, (bar+rh) );
	
	if ((widd > bar) && (heii > bar))	
		outcol = vec4(0,0,1,1);
	else
		outcol = vec4(0.4,0.4,0.4,1);
		
    gl_FragColor = outcol;
}
