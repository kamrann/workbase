// Matrices
uniform mat4 u_mvp;

// Per vertex position, normal and color
attribute vec4 a_position;
attribute vec4 a_color;

// Color value to be interpolated
varying vec4 v_color;

void main()
{
	// Set vertex color
	v_color = a_color;
	// Transform the vertex position into projection space
	gl_Position = u_mvp * a_position;
}
