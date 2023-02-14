#ifndef VICON_PLUGIN_HPP
#define VICON_PLUGIN_HPP

#include "DataStreamClient.h"
#include <iostream>

namespace dls
{
    class ViconPlugin
    {
    public:
        ViconPlugin();
        ~ViconPlugin();

        /**
         * @brief Get the Vicon DataStream SDK version
         * @return const int
         */
        void getVersion();
    };
} // end namespace dls
#endif /* end of include guard: VICON_PLUGIN_HPP */
