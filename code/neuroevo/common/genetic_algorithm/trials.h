// trials.h

#ifndef __WB_GA_TRIALS_H
#define __WB_GA_TRIALS_H


namespace ga {

	struct trials_cfg
	{
		enum class MergeMethod {
			Average,
			Worst,
		};

		size_t num_trials;
		MergeMethod merging;
	};
	
}


#endif


