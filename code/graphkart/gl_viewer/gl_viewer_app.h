// gl_viewer_app.h

#include "util/camera.h"

#include "gles_angle/esUtil.h"
#include "gles_angle/shader_program.h"
#include "util/graphics3d/mesh.h"

#include <Eigen/Dense>


class gl_viewer_app
{
public:
	gl_viewer_app();
    ~gl_viewer_app();

	bool setup_shaders();
	bool initialise(ESContext* context);

	void process_keyboard_input(unsigned char c, int x, int y);
	void process_mouse_movement(int x, int y);

	void rotate_object_world(Eigen::Quaternionf const& q);
	void rotate_object_local(Eigen::Quaternionf const& q);

	camera& active_camera();

	void update(ESContext* context, float dtime);

/*	void draw_basis(
		Eigen::Vector3f const& pos = Eigen::Vector3f(0, 0, 0),
		Eigen::Quaternionf const& orient = Eigen::Quaternionf::Identity(),
		Eigen::Vector4f const& color = Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
*/	void draw_object();
	void draw(ESContext* context);

public:
	enum {
		SCREEN_WIDTH = 800,
		SCREEN_HEIGHT = 600,
	};

private:
	shader_prog tex_shader;
	shader_prog vcolor_shader;
	shader_prog vcolor_unlit_shader;

	GLuint tex_obj_id;

	camera cam;
	camera secondary_cam;

	mesh< float > box;
	mesh< float > sphere;
	mesh< float > cylinder;
	mesh< float > torus;
	mesh< float > cone;

	mesh< float > spline;
	std::vector< Eigen::Vector3f > spline_points;
	std::vector< Eigen::Vector3f > spline_tangents;
	std::vector< Eigen::Vector3f > spline_normals;

	mesh< float > camera_mesh;

	Eigen::Vector3f obj_position;
	Eigen::Quaternionf obj_orientation;

	Eigen::Quaternionf slerp_start;
	Eigen::Quaternionf slerp_end;
	float slerp_pos;
	bool slerping;

	bool secondary_cam_view;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
