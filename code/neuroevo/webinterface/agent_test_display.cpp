// agent_test_display.cpp

#include "agent_test_display.h"

#include <Wt/WMatrix4x4>


std::vector< float > vdata;


agent_test_display::agent_test_display(Wt::WContainerWidget *root, bool const& use_binary_buffers) : WGLWidget(root)
{

}

void agent_test_display::initializeGL()
{
	std::string const vertex_shader_src =
		"	// Matrices														\n"
		"	uniform mat4 u_mvp;												\n"

		"	// Per vertex position, normal and color						\n"
		"	attribute vec4 a_position;										\n"
		"	attribute vec4 a_color;											\n"

		"	// Color value to be interpolated								\n"
		"	varying vec4 v_color;											\n"

		"	void main()														\n"
		"	{																\n"
		"		// Set vertex color											\n"
		"		v_color = a_color;											\n"
		"		// Transform the vertex position into projection space		\n"
		"		gl_Position = u_mvp * a_position;							\n"
		"	}																\n"
		;

	std::string const frag_shader_src =
		"	precision mediump float;					\n"

		"	varying vec4 v_color;						\n"

		"	void main()									\n"
		"	{											\n"
		"		gl_FragColor = v_color;					\n"
		"	}											\n"
		;

	Shader frag_shader = createShader(FRAGMENT_SHADER);
	shaderSource(frag_shader, frag_shader_src);
	compileShader(frag_shader);
	Shader vertex_shader = createShader(VERTEX_SHADER);
	shaderSource(vertex_shader, vertex_shader_src);
	compileShader(vertex_shader);
	shader_program = createProgram();
	attachShader(shader_program, vertex_shader);
	attachShader(shader_program, frag_shader);
	linkProgram(shader_program);
	useProgram(shader_program);

	position_attrib_loc = getAttribLocation(shader_program, "a_position");
	enableVertexAttribArray(position_attrib_loc);
	color_attrib_loc = getAttribLocation(shader_program, "a_color");
	vertexAttrib3f(color_attrib_loc, 1.0, 0.0, 0.0);

	// Extract the references the uniforms from the shader
	mvp_uniform_loc = getUniformLocation(shader_program, "u_mvp");

	Wt::WMatrix4x4 mvp;
	mvp.ortho(-1.0, 1.0, -1.0, 1.0, -100.0, 100.0);
	uniformMatrix4(mvp_uniform_loc, mvp);

//	js_model_xform = createJavaScriptMatrix4();

	obj_buffer = createBuffer();
	bindBuffer(ARRAY_BUFFER, obj_buffer);

	vdata.resize(9);
	vdata[0] = 0.0f;
	vdata[1] = 0.0f;
	vdata[2] = 0.0f;
	vdata[3] = 0.5f;
	vdata[4] = 0.0f;
	vdata[5] = 0.0f;
	vdata[6] = 0.5f;
	vdata[7] = 0.5f;
	vdata[8] = 0.0f;
	bufferDatafv(ARRAY_BUFFER, vdata.begin(), vdata.end(), STATIC_DRAW);

	clearColor(0, 0, 0, 1.0);
	disable(CULL_FACE);
}

void agent_test_display::resizeGL(int width, int height)
{
	viewport(0, 0, width, height);
}

void agent_test_display::paintGL()
{
	clear(COLOR_BUFFER_BIT);

	bindBuffer(ARRAY_BUFFER, obj_buffer);
	vertexAttribPointer(position_attrib_loc,
		3,
		FLOAT,
		false,
		3 * 4,
		0);
	drawArrays(TRIANGLES, 0, vdata.size() / 3);
}

