// neuron_filter.cpp

#include "neuron_filter.h"
#include "neuron.h"


namespace nnet {

	const neuron_filter neuron_filter::Any = { {}, neuron_filter::Mode::Exclude };
	const neuron_filter neuron_filter::Input = { { NeuronType::Input } };
	const neuron_filter neuron_filter::Output = { { NeuronType::Output } };
	const neuron_filter neuron_filter::Hidden = { { NeuronType::Hidden } };
	const neuron_filter neuron_filter::Bias = { { NeuronType::Bias } };
	const neuron_filter neuron_filter::NonBias = { { NeuronType::Bias }, neuron_filter::Mode::Exclude };
	const neuron_filter neuron_filter::HasActivation = { { NeuronType::Input, NeuronType::Bias }, neuron_filter::Mode::Exclude };

	bool neuron_filter::test(neuron_data const& nr) const
	{
		switch(mode)
		{
			case Mode::Include:
			if(types.find(nr.type) == types.end())
			{
				return false;
			}
			break;
			case Mode::Exclude:
			if(types.find(nr.type) != types.end())
			{
				return false;
			}
			break;
		}

		// TODO:

		return true;
	}

}



