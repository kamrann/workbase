// proto_app.h

#include "level_graph.h"
#include "agent.h"
#include "ingame_camera.h"

#include "graph_defs/graph_defn.h"
#include "graphgen/gen_spatial.h"

#include "util/graphics3d/mesh.h"

#include "gles_angle/esUtil.h"
#include "gles_angle/shader_program.h"

#include <Eigen/Dense>

#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_ptr.hpp>

#include <map>


class proto_app
{
public:
	proto_app();
    ~proto_app();

	bool setup_shaders();
	void build_node_mesh();
	void build_connection_mesh();
	void build_player_meshes();
	void build_static_meshes();
	void build_graph_dependent_meshes();
	bool initialise(ESContext* context);

	void process_keyboard_input(unsigned char c, int x, int y);
	void process_mouse_movement(int x, int y);

	camera const& active_camera() const;
	void reset_free_camera();
	void switch_to_third_person();
	void switch_to_first_person();

	bool players_active() const;
	void restart_players();
	void remove_players();
	void new_game();
	void toggle_pause();
	void update(ESContext* context, float dtime);

	void draw_connections();
	void draw_nodes();
	void draw_players();
	void draw(ESContext* context);

public:
	enum {
		SCREEN_WIDTH = 1024,
		SCREEN_HEIGHT = 768,
	};

private:
	boost::random::mt19937 rgen;

	level_graph level;
	std::vector< agent > players;

	camera free_cam;
	boost::shared_ptr< ingame_camera > ingame_cam;

	bool paused;

	shader_prog tex_shader, vcolor_shader, vcolor_unlit_shader;
	GLuint tex_obj_id;
	bool texturing_on;
	bool lighting_on;

	bool draw_simple_splines;

	mesh< float > node_mesh;
	mesh< float > connection_mesh;
	mesh< float > player_inner_mesh;
	mesh< float > player_outer_mesh;

	std::vector< mesh< float > > spline_meshes;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
