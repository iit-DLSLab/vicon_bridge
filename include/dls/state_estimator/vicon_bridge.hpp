#ifndef VICON_BRIDGE_HPP
#define VICON_BRIDGE_HPP

// Vicon DataStream SDK header
#include <DataStreamClient.h>

#include "dls2/estimator/estimator.hpp"
#include "dls2/signal/writer.hpp"
#include "dls_messages/dds/vicon.hpp"

namespace dls
{
    namespace state_estimator 
    {
        class ViconBridge : public Estimator
        {
        public:
            ViconBridge(std::string& ID);
            ~ViconBridge();

            void run(const std::chrono::system_clock::time_point&) override;

            AppStatus eStop() override {return getStatus();}
            std::string where() override {return "No info from here yet!";};

            /**
             * @brief Establish a connection between the ViconDataStreamSDK client and the Vicon Nexus and 
             * configure the client and the options that you want to enable for the Vicon data.
             * See "ViconDataStreamSDK_CPPTest.cpp" for more info and options.
             */
            void connectToVicon();

            /**
             * @brief Terminate the connection between the ViconDataStreamSDK client and the Vicon Nexus
             */
            void disconnectFromVicon();

            /**
             * @brief Get the Vicon DataStream SDK version
             */
            void getVersion();

            /**
             * @brief Get the information relative to the Vicon data flow
             */
            void getConnectionDataInfo();

            /**
             * @brief Get the robot position (Global Translation)
             */
            void getRobotPosition(ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation& segment_global_translation);

            /**
             * @brief Get the robot orientation (Global Rotation Quaternion)
             */
            void getRobotOrientation(ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationQuaternion& segment_global_rotation_quaternion);

            /**
             * @brief Get the markers position (Marker Global Translation)
             */
            void getMarkersPositions(std::vector<ViconDataStreamSDK::CPP::Output_GetMarkerGlobalTranslation>& markers_global_translations);

        private:
            WriterPtr<dls2_interface::msg::Vicon> writer_vicon_;

            ViconDataStreamSDK::CPP::Client client_;

            std::string subject_name_{}, segment_name_{};

            // Variable to show robot pose, markers position and other information on the terminal
            bool print_on_screen_{false};
        };
    } // namespace state_estimator
} // namespace dls
#endif /* VICON_BRIDGE_HPP */