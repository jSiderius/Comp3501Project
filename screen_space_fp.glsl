#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;
uniform float fill;

void main() 
{
vec4 pixel;
vec2 pos = uv0;
pixel = texture(texture_map,uv0);
	
	// Calculate distance to center
    float distance = length(uv0 - vec2(0.5, 0.5));
	float modifier = fill * (1.0 - (distance*distance));
	modifier = max(min(modifier, 1.0), 0.0);
	pixel = vec4(pixel.r*modifier,pixel.g*modifier,pixel.b*modifier,1);
	


  //gl_FragColor = (pixel+pixel2)/2;
  gl_FragColor = pixel;
}
