// shader_program.h

#ifndef __GLES_UTIL_SHADER_PROGRAM_H
#define __GLES_UTIL_SHADER_PROGRAM_H

#include <Eigen/Dense>

#include <GLES2\gl2.h>

#include <array>
#include <string>
#include <map>


class shader_prog
{
private:
	GLuint		program_id;

	enum StandardAttrib {
		atPosition = 0,
		atNormal,
		atTexCoord,
		atColor,

		NumStandardAttributes
	};

	enum StandardUniform {
		ufMVP = 0,
		ufMV,
		ufMVNorm,
		ufLightVec,
		ufTexUnit,

		NumStandardUniforms
	};

	typedef std::map< std::string, int > standard_attrib_map_t, standard_uniform_map_t;

	static const standard_attrib_map_t StandardAttribNames;
	static const standard_uniform_map_t StandardUniformNames;

	//std::array< GLint, NumStandardAttributes >	standard_attrib_locs;
	std::array< GLint, NumStandardUniforms >	standard_uniform_locs;

public:
	shader_prog();
	~shader_prog();

private:
	bool load_shader(GLuint shader, std::string src);

	void bind_standard_attributes();
	void retrieve_uniform_locations();

//	inline void set_fixed_attrib(GLint loc, Eigen::Matrix< float, 4, 4 > const& m);
	inline void set_fixed_attrib(GLint loc, Eigen::Matrix< float, 2, 1 > const& v);
	inline void set_fixed_attrib(GLint loc, Eigen::Matrix< float, 3, 1 > const& v);
	inline void set_fixed_attrib(GLint loc, Eigen::Matrix< float, 4, 1 > const& v);

	inline void load_attrib_array(GLint loc, GLint components, const GLfloat* data);
	inline void disable_attrib_array(GLint loc);

	inline void load_uniform(GLint loc, Eigen::Affine3f const& xf);
		//Eigen::Matrix< float, 4, 4 > const& m);
	inline void load_uniform(GLint loc, Eigen::Projective3f const& xf);
	inline void load_uniform(GLint loc, Eigen::Matrix< float, 3, 1 > const& v);
	inline void load_uniform(GLint loc, GLenum val);

public:
	GLuint get_id() const;
	bool is_loaded() const;
	bool load(std::string vert_sh, std::string frag_sh);
	void unload();
	void activate();

	// TODO: Not sure stuff like this should be part of program class, since they are not part of program state,
	// but rather part of overall opengl state...
	inline void set_fixed_vertex_normal(Eigen::Vector3f const& n);
	inline void set_fixed_vertex_tex_coords(Eigen::Vector2f const& t);
	inline void set_fixed_vertex_color(Eigen::Vector4f const& c);

	inline void load_vertices(const GLfloat* v);
	inline void load_normals(const GLfloat* n);
	inline void load_tex_coords(const GLfloat* t);
	inline void load_colors(const GLfloat* c);

	inline void disable_vertex_array();
	inline void disable_normal_array();
	inline void disable_tex_coord_array();
	inline void disable_color_array();

	inline void load_mvp_matrix(Eigen::Projective3f const& mvp);
		//Eigen::Matrix< float, 4, 4 > const& mvp);
	inline void load_mv_matrix(Eigen::Affine3f const& mv);
		//Eigen::Matrix< float, 4, 4 > const& mv);
	inline void load_mv_normals_matrix(Eigen::Affine3f const& mvnorm);
		//Eigen::Matrix< float, 4, 4 > const& mv_normals);
	inline void load_light_vector(Eigen::Matrix< float, 3, 1 > const& light_vec);
	inline void load_texture_unit(GLenum tex_unit);
};


inline void shader_prog::set_fixed_attrib(GLint loc, Eigen::Matrix< float, 2, 1 > const& v)
{
	glVertexAttrib2fv(loc, v.data());
}

inline void shader_prog::set_fixed_attrib(GLint loc, Eigen::Matrix< float, 3, 1 > const& v)
{
	glVertexAttrib3fv(loc, v.data());
}

inline void shader_prog::set_fixed_attrib(GLint loc, Eigen::Matrix< float, 4, 1 > const& v)
{
	glVertexAttrib4fv(loc, v.data());
}

inline void shader_prog::set_fixed_vertex_normal(Eigen::Vector3f const& n)
{
	set_fixed_attrib(atNormal, n);
}

inline void shader_prog::set_fixed_vertex_tex_coords(Eigen::Vector2f const& t)
{
	set_fixed_attrib(atTexCoord, t);
}

inline void shader_prog::set_fixed_vertex_color(Eigen::Vector4f const& c)
{
	set_fixed_attrib(atColor, c);
}

inline void shader_prog::load_attrib_array(GLint loc, GLint components, const GLfloat* data)
{
	glVertexAttribPointer(loc, components, GL_FLOAT, GL_FALSE, 0, data);
	glEnableVertexAttribArray(loc);
}

inline void shader_prog::disable_attrib_array(GLint loc)
{
	glDisableVertexAttribArray(loc);
}

inline void shader_prog::load_vertices(const GLfloat* v)
{
	load_attrib_array(atPosition, 3, v);
}

inline void shader_prog::load_normals(const GLfloat* n)
{
	load_attrib_array(atNormal, 3, n);
}

inline void shader_prog::load_tex_coords(const GLfloat* t)
{
	load_attrib_array(atTexCoord, 2, t);
}

inline void shader_prog::load_colors(const GLfloat* c)
{
	load_attrib_array(atColor, 4, c);
}

inline void shader_prog::disable_vertex_array()
{
	disable_attrib_array(atPosition);
}

inline void shader_prog::disable_normal_array()
{
	disable_attrib_array(atNormal);
}

inline void shader_prog::disable_tex_coord_array()
{
	disable_attrib_array(atTexCoord);
}

inline void shader_prog::disable_color_array()
{
	disable_attrib_array(atColor);
}

inline void shader_prog::load_uniform(GLint loc, Eigen::Affine3f const& xf)
									  //Eigen::Matrix< float, 4, 4 > const& m)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, xf.data());
}

inline void shader_prog::load_uniform(GLint loc, Eigen::Projective3f const& xf)
{
	glUniformMatrix4fv(loc, 1, GL_FALSE, xf.data());
}

inline void shader_prog::load_uniform(GLint loc, Eigen::Matrix< float, 3, 1 > const& v)
{
	glUniform3fv(loc, 1, v.data());
}

inline void shader_prog::load_uniform(GLint loc, GLenum val)
{
	glUniform1i(loc, val);
}

inline void shader_prog::load_mvp_matrix(Eigen::Projective3f const& mvp)
//	Eigen::Matrix< float, 4, 4 > const& mvp)
{
	load_uniform(standard_uniform_locs[ufMVP], mvp);
}

inline void shader_prog::load_mv_matrix(Eigen::Affine3f const& mv)
//	Eigen::Matrix< float, 4, 4 > const& mv)
{
	load_uniform(standard_uniform_locs[ufMV], mv);
}

inline void shader_prog::load_mv_normals_matrix(Eigen::Affine3f const& mvnorm)
//	Eigen::Matrix< float, 4, 4 > const& mvnorm)
{
	load_uniform(standard_uniform_locs[ufMVNorm], mvnorm);
}

inline void shader_prog::load_light_vector(Eigen::Matrix< float, 3, 1 > const& light_vec)
{
	load_uniform(standard_uniform_locs[ufLightVec], light_vec);
}

inline void shader_prog::load_texture_unit(GLenum tex_unit)
{
	load_uniform(standard_uniform_locs[ufTexUnit], tex_unit);
}



#endif



