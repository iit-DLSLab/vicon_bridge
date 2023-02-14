#include "vicon_plugin.hpp"

namespace dls
{
    ViconPlugin::ViconPlugin(){ std::cout << "Vicon Plugin - Constructor" << std::endl; }
    ViconPlugin::~ViconPlugin(){ std::cout << "Vicon Plugin - Destructor" << std::endl; }

    void ViconPlugin::getVersion()
    {
        ViconDataStreamSDK::CPP::Client client;
        ViconDataStreamSDK::CPP::Output_GetVersion output_get_version = client.GetVersion();
        std::cout << "Vicon DataStream SDK version: " << output_get_version.Major << "."
                                                      << output_get_version.Minor << "."
                                                      << output_get_version.Point << "."
                                                      << output_get_version.Revision << std::endl;
    }
}