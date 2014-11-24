// wt_server.h

#ifndef __WB_WTDISP_WTSERVER_H
#define __WB_WTDISP_WTSERVER_H

#include "display_types.h"
#include "display_defn.h"

#include <string>
#include <vector>
#include <functional>
#include <memory>


typedef std::vector< std::string > arg_list;

bool start_wt_server(arg_list args);
void terminate_wt_server();

class i_terminal
{
public:
	typedef std::function< void(std::string) > cmd_handler_fn;

public:
	virtual void register_command_handler(cmd_handler_fn handler) = 0;
	virtual void output(std::string text) = 0;
	virtual void set_prompt(std::string prompt) = 0;
};

i_terminal* create_wt_terminal(std::string name);

bool register_display_defn(std::unique_ptr< display_defn > defn);
unsigned long create_wt_display(display_data data);
display_data& get_display_data(unsigned long id);
void release_display_data();
void update_display(unsigned long id);



#endif


