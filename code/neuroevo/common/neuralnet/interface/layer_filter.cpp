// layer_filter.cpp

#include "layer_filter.h"


namespace nnet {

	const layer_filter layer_filter::Any = { {}, layer_filter::Mode::Exclude };
	const layer_filter layer_filter::Input = { { LayerType::Input } };
	const layer_filter layer_filter::Output = { { LayerType::Output } };
	const layer_filter layer_filter::Hidden = { { LayerType::Hidden } };

	bool layer_filter::test(layer_data const& ly) const
	{
		switch(mode)
		{
			case Mode::Include:
			//if(types.find(ly) == types.end())
			if(std::find_if(
				std::begin(types),
				std::end(types),
				[&ly](layer_data const& ld)
			{
				return ld.match(ly);
			}
				) == types.end())
			{
				return false;
			}
			break;
			case Mode::Exclude:
			//if(types.find(ly) != types.end())
			if(std::find_if(
				std::begin(types),
				std::end(types),
				[&ly](layer_data const& ld)
			{
				return ld.match(ly);
			}
				) != types.end())
			{
				return false;
			}
			break;
		}

		return true;
	}

}



