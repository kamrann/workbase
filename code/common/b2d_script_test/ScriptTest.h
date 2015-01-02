/*
Test case for box2d scripts.
*/

#ifndef SCRIPT_TEST_H
#define SCRIPT_TEST_H

#include "b2d_script/parsing/load_script.h"
#include "Testbed/Framework/Test.h"

#include <vector>
#include <iostream>
#include <string>


struct script
{
	std::string name;
	std::string filepath;
};

extern std::vector< script > b2d_scripts;

class ScriptTest;

typedef ScriptTest* (*ScriptTestCreateFcn)(std::string);

struct ScriptTestEntry
{
	const char *name;
	ScriptTestCreateFcn createFcn;
};


class ScriptTest : public Test
{
public:
	ScriptTest(std::string script_filepath)
	{
		if(!b2ds::load_script_from_file(script_filepath, m_world))
		{
			assert(false);
		}

		b2Body* ground = NULL;
		{
			b2BodyDef bd;
			ground = m_world->CreateBody(&bd);

			double const GroundWidth = 100.0;

			{
				b2EdgeShape shape;
				shape.Set(b2Vec2(-GroundWidth / 2, 0.0), b2Vec2(GroundWidth / 2, 0.0));

				b2FixtureDef fd;
				fd.shape = &shape;
				fd.density = 0.0f;
				fd.friction = 0.5f;

				ground->CreateFixture(&fd);
			}
		}

		g_camera.m_center = b2Vec2(0.0, 0.0);
		g_camera.m_zoom = 0.4;
		m_fixed_camera = true;
	}

	void Keyboard(int key)
	{
		switch (key)
		{
			case GLFW_KEY_C:
			m_fixed_camera = !m_fixed_camera;
			if(m_fixed_camera)
			{
				g_camera.m_center = b2Vec2(0.0, 0.0);
			}
			break;
		}
	}

	void Step(Settings* settings)
	{
		g_debugDraw.DrawString(5, m_textLine, "Keys: c = Camera fixed/tracking");
		m_textLine += DRAW_STRING_NEW_LINE;

		if(!m_fixed_camera)
		{
//			g_camera.m_center = m_skier->GetPosition();
		}

		Test::Step(settings);
	}

	static ScriptTest* Create(std::string script_filepath)
	{
		return new ScriptTest{ script_filepath };
	}

	bool m_fixed_camera;
};


#endif
