// system_sim.h

#ifndef __WB_PW_EXECUTORS_SYSTEM_SIM_H
#define __WB_PW_EXECUTORS_SYSTEM_SIM_H

#include "peerwork/peerwork/executor_loading.h"


extern "C" {

	__declspec(dllexport) char const* pw_ex_get_name();
	__declspec(dllexport) char* pw_ex_run(char const* spec, void* context, pwork::pw_ex_update_cb_fn update_fn, pwork::pw_ex_buffer_alloc_fn alloc);

}


#endif



