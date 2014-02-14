// agent_test_display.h

#ifndef __AGENT_TEST_DISPLAY_H
#define __AGENT_TEST_DISPLAY_H

#undef NO_ERROR
#include <Wt/WGLWidget>


class agent_test_display: public Wt::WGLWidget
{
private:
	Program shader_program;
	AttribLocation position_attrib_loc;
	AttribLocation color_attrib_loc;
	UniformLocation mvp_uniform_loc;
	Buffer obj_buffer;
	JavaScriptMatrix4x4 js_model_xform;

public:
	agent_test_display(Wt::WContainerWidget *root, bool const& use_binary_buffers);

public:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
};


#endif


