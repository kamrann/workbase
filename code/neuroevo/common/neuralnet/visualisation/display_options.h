// display_options.h

#ifndef __WB_NN_VIS_DISPLAY_OPTIONS_H
#define __WB_NN_VIS_DISPLAY_OPTIONS_H


namespace nnet {

	namespace vis {

		enum class DisplayMode {
			Structure,
			// Edit?
			State,
		};

		enum DisplayFlags {
			ActivationFunction = 1 << 0,
			Connections = 1 << 1,
			ColourWeights = 1 << 2,
			SignalGradients = 1 << 3,
		};

		struct DisplayOptions
		{
			uint32_t options;

			DisplayOptions(uint32_t opts = 0):
				options(opts)
			{}

			inline bool test(uint32_t flag) const
			{
				return (options & flag) != 0;
			}

			inline void set(uint32_t flag)
			{
				options |= flag;
			}

			inline void clear(uint32_t flag)
			{
				options &= ~flag;
			}

			inline void toggle(uint32_t flag, bool on)
			{
				if(on)
				{
					set(flag);
				}
				else
				{
					clear(flag);
				}
			}

			static const uint32_t Default = ActivationFunction | Connections | ColourWeights | SignalGradients;
		};

	}
}


#endif


