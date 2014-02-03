// Matrices
uniform mat4 u_mvp;
//uniform mat4 u_mv;
uniform mat4 u_mv_normals;

// Reverse directional light direction vector in view space, magnitude denotes strength
uniform vec3 u_light_vec;

// Per vertex position, normal and color
attribute vec4 a_position;
attribute vec3 a_normal;
attribute vec2 a_tex_coord;

// Color value to be interpolated
varying float v_diffuse;
varying vec2 v_tex_coord;

void main()
{
	// Transform vertex in view space
//	vec3 vs_position = vec3(u_mv * a_position);
	// And the normal
	vec3 vs_normal = normalize(vec3(u_mv_normals * vec4(a_normal, 0.0)));
	// Calculate incident light strength
	v_diffuse = max(dot(u_light_vec, vs_normal), 0.1);
	// Set tex coord to interpolate
	v_tex_coord = a_tex_coord;
	// Transform the vertex position into projection space
	gl_Position = u_mvp * a_position;
}
