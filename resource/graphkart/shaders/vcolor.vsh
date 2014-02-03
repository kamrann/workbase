// Matrices
uniform mat4 u_mvp;
uniform mat4 u_mv_normals;

// Vector to light, in view space 
uniform vec3 u_light_vec;

// Per vertex position, normal and color
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec4 a_color;

// Color value to be interpolated
varying vec4 v_color;

void main()
{
	// Calculate diffuse lighting intensity
	vec3 vs_normal = normalize(vec3(u_mv_normals * vec4(a_normal, 0.0)));
	float diffuse = max(dot(vs_normal, u_light_vec), 0.1);
	// Set vertex color
	v_color = a_color * diffuse;
	// Transform the vertex position into projection space
	gl_Position = u_mvp * a_position;
}
