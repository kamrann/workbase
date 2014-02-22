// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "test.h"

#include <Wt/WApplication>


Wt::WApplication* createApplication(const Wt::WEnvironment& env)
{
	return new Wt::WApplication(env);
}

int main(int argc, char* argv[])
{
	return Wt::WRun(argc, argv, &createApplication);
}

