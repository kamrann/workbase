// shader_program.cpp

#include "shader_program.h"

#include <boost/assign/list_of.hpp>


const shader_prog::standard_attrib_map_t shader_prog::StandardAttribNames = boost::assign::map_list_of< std::string, int >
	("a_position", shader_prog::atPosition)
	("a_normal", shader_prog::atNormal)
	("a_tex_coord", shader_prog::atTexCoord)
	("a_color", shader_prog::atColor)
	;

const shader_prog::standard_uniform_map_t shader_prog::StandardUniformNames = boost::assign::map_list_of< std::string, int >
	("u_mvp", shader_prog::ufMVP)
	("u_mv", shader_prog::ufMV)
	("u_mv_normals", shader_prog::ufMVNorm)
	("u_light_vec", shader_prog::ufLightVec)
	("u_tex_unit", shader_prog::ufTexUnit)
	;


shader_prog::shader_prog():
	standard_uniform_locs()
{
	program_id = 0;
}

shader_prog::~shader_prog()
{
	unload();
}

GLuint shader_prog::get_id() const
{
	return program_id;
}

bool shader_prog::is_loaded() const
{
	return program_id != 0;
}

bool shader_prog::load_shader(GLuint shader, std::string src)
{
	// Check that shader object is valid
	if(shader == 0)
	{
		return false;
	}

	// Load the shader source
	const char* str = src.c_str();
	glShaderSource(shader, 1, &str, nullptr);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if(compiled == 0)
	{
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if(infoLen > 1)
		{
			char* infoLog = new char[infoLen];

			glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
			//esLogMessage ( "Error compiling shader:\n%s\n", infoLog );            

			delete[] infoLog;
		}

		glDeleteShader(shader);
		return false;
	}

	return true;
}

bool shader_prog::load(std::string vert_sh, std::string frag_sh)
{
	unload();

	// Load the vertex/fragment shaders
	GLuint vtx_shader_id = glCreateShader(GL_VERTEX_SHADER);
	if(!load_shader(vtx_shader_id, vert_sh))
	{
		glDeleteShader(vtx_shader_id);
		return false;
	}

	GLuint frg_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	if(!load_shader(frg_shader_id, frag_sh))
	{
		glDeleteShader(vtx_shader_id);
		glDeleteShader(frg_shader_id);
		return false;
	}

	// Create the program object
	program_id = glCreateProgram();
	if(program_id == 0)
	{
		glDeleteShader(vtx_shader_id);
		glDeleteShader(frg_shader_id);
		return false;
	}

	glAttachShader(program_id, vtx_shader_id);
	glAttachShader(program_id, frg_shader_id);

	// Bind standard attributes
	bind_standard_attributes();

	// Link the program
	glLinkProgram(program_id);

	// Free up no longer needed shader resources
	glDeleteShader(vtx_shader_id);
	glDeleteShader(frg_shader_id);

	// Check the link status
	GLint linked;
	glGetProgramiv(program_id, GL_LINK_STATUS, &linked);

	if(linked == 0)
	{
		GLint infoLen = 0;

		glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &infoLen);

		if(infoLen > 1)
		{
			char* infoLog = new char[infoLen];

			glGetProgramInfoLog(program_id, infoLen, nullptr, infoLog);
			//esLogMessage( "Error linking program:\n%s\n", infoLog);

			delete[] infoLog;
		}

		unload();
		return false;
	}

	retrieve_uniform_locations();

	return true;
}

void shader_prog::unload()
{
	if(program_id != 0)
	{
		glDeleteProgram(program_id);
		program_id = 0;
	}
}

void shader_prog::activate()
{
	glUseProgram(program_id);
}

void shader_prog::bind_standard_attributes()
{
	for(auto const& v: StandardAttribNames)
	{
		glBindAttribLocation(program_id, v.second, v.first.c_str());
	}
}

void shader_prog::retrieve_uniform_locations()
{
	GLint num_active_uniforms;
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &num_active_uniforms);

	for(size_t i = 0; i < num_active_uniforms; ++i)
	{
		size_t const MaxNameLength = 64;
		char uf_name[MaxNameLength + 1];
		GLint uf_size;
		GLenum uf_type;
		glGetActiveUniform(program_id, i, MaxNameLength + 1, nullptr, &uf_size, &uf_type, uf_name);

		standard_uniform_map_t::const_iterator it = StandardUniformNames.find(std::string(uf_name));
		if(it != StandardUniformNames.end())
		{
			// This is a standard uniform
			standard_uniform_locs[it->second] = glGetUniformLocation(program_id, it->first.c_str());
		}
		else
		{
			// Non-standard uniform
			// TODO:
		}
	}
}

