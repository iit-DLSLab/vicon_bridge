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

#ifndef VICON_BRIDGE_HPP
#define VICON_BRIDGE_HPP

// Vicon DataStream SDK header
#include "DataStreamClient.h"

#include "dls2/estimator/estimator.hpp"
#include "dls2/msg_wrappers/signal_reader.hpp"
#include "dls2/msg_wrappers/signal_writer.hpp"
#include "dls/parameter_server/parameter_client.hpp"

namespace dls
{
    class ViconBridge : public Estimator
    {
    public:
        ViconBridge(std::string& ID);
        ~ViconBridge();

        void run(const std::chrono::system_clock::time_point&) override;

        Status eStop() override {return getStatus();}
        std::string where() override {return "No info from here yet!";};

        /**
         * @brief Get the Vicon DataStream SDK version
         */
        void getVersion();
    private:
    };
} // end namespace dls
#endif /* end of include guard: VICON_BRIDGE_HPP */