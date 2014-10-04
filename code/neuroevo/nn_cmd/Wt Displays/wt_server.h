// wt_server.h

#ifndef __WB_NNCMD_WTDISP_WTSERVER_H
#define __WB_NNCMD_WTDISP_WTSERVER_H

#include "display_types.h"

#include "../basic_types.h"


bool start_wt_server(arg_list args);
void terminate_wt_server();

void create_wt_display(display_data data);


#endif


