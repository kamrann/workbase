// wt_server.h

#ifndef __WB_WTDISP_WTSERVER_H
#define __WB_WTDISP_WTSERVER_H

#include "display_types.h"

#include <string>
#include <vector>


typedef std::vector< std::string > arg_list;

bool start_wt_server(arg_list args);
void terminate_wt_server();

unsigned long create_wt_display(display_data data);
display_data& get_display_data(unsigned long id);
void release_display_data();
void update_display(unsigned long id);



#endif


