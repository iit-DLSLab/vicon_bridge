/*******************************************************************************
*                                                       ,----,                 *
*                                                     .'   .' \                *
*                                                   ,----,'    |               *
*               ________  ___       ________        |    :  .  ;               *
*              |\   ___ \|\  \     |\   ____\       ;    |.'  /                *
*              \ \  \_|\ \ \  \    \ \  \___|_      `----'/  ;                 *
*               \ \  \ \\ \ \  \    \ \_____  \       /  ;  /                  *
*                \ \  \_\\ \ \  \____\|____|\  \     ;  /  /-,                 *
*                 \ \_______\ \_______\____\_\  \   /  /  /.`|                 *
*                  \|_______|\|_______|\_________\./__;      :                 *
*                                     \|_________||   :    .'                  *
*                                                 ;   | .'                     *
*                                                 `---'                        *
*******************************************************************************/

#include "dls/state_estimator/vicon_bridge.hpp"

namespace dls
{
    namespace state_estimator 
    {
        ViconBridge::ViconBridge(std::string& ID)
            : Estimator(
                ID, 
                std::chrono::milliseconds(10))
            { 
                std::cout << "Vicon Plugin - Constructor" << std::endl;
            }

        ViconBridge::~ViconBridge()
        {
            std::cout << "Vicon Plugin - Destructor" << std::endl;
        }

        void ViconBridge::run(const std::chrono::system_clock::time_point& time)
        {
            std::cout << "Run method" << std::endl;
            getVersion();
        }

        void ViconBridge::getVersion()
        {
            ViconDataStreamSDK::CPP::Client client;
            ViconDataStreamSDK::CPP::Output_GetVersion output_get_version = client.GetVersion();
            std::cout << "Vicon DataStream SDK version: " << output_get_version.Major << "."
                                                        << output_get_version.Minor << "."
                                                        << output_get_version.Point << "."
                                                        << output_get_version.Revision << std::endl;
        }

        extern "C" ViconBridge *create(std::string ID)
        {
            return new ViconBridge(ID);
        }

        extern "C" void destroy(ViconBridge *p)
        {
            delete p;
        }
    } //namespace state_estimator
} //namespace dls