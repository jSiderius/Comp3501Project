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
    float rectangleHeight = 0.04; 
    float rectangleWidth = 0.02; 

    vec4 outcol; 

    int row_index = int((uv_interp.x - mod(uv_interp.x, rectangleWidth)) / rectangleWidth);
    int col_index = int((uv_interp.y - mod(uv_interp.y, rectangleHeight)) / rectangleHeight);

    float adj_sec = 10;
    float red_adjust; 
    if(mod(timer,2*adj_sec) < adj_sec)
        red_adjust = mod(timer,adj_sec) / adj_sec;
    else
        red_adjust = 1.0 - mod(timer,adj_sec) / adj_sec;

    if ((row_index % 2 == 0 && col_index % 2 == 0) || (row_index % 2 != 0 && col_index % 2 != 0))
        outcol = vec4(0,1,0,1);
	else
		outcol = vec4(red_adjust,0.1,0.1,1);

    gl_FragColor = outcol;
}



