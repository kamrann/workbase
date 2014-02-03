precision mediump float;

varying vec4 v_color;

uniform sampler2D u_tex_unit;
varying float v_diffuse;
varying vec2 v_tex_coord;

void main()
{
	vec4 color = texture2D(u_tex_unit, v_tex_coord);
	gl_FragColor = color * vec4(v_diffuse, v_diffuse, v_diffuse, 1.0);
}
