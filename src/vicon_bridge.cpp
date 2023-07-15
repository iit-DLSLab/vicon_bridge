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
                ID)
                , writer_vicon_(
                this->getParticipant(),
                dls::topics::high_level_estimation::vicon,
                std::make_shared<Vicon>()
                )
            { 
                connectToVicon();
            }

        ViconBridge::~ViconBridge()
        {
            disconnectFromVicon();
        }

        void ViconBridge::run(const std::chrono::system_clock::time_point& time)
        {
            // Get a frame
            while(client_.GetFrame().Result != ViconDataStreamSDK::CPP::Result::Success)
            {
                std::cout << "\nWaiting for a frame...\n" << std::endl;

                // Sleep a little so that we don't lumber the CPU with a busy poll
                sleep(1);
            }

            // Fill the ViconMsg "timestamp" field
            writer_vicon_->timestamp = time.time_since_epoch().count();

            // Get the subject name (Important: We are considering here only a single subject)
            subject_name_ = client_.GetSubjectName(0).SubjectName;

            // Get the segment name (Important: We are considering here only a subject with a single segment)
            segment_name_ = client_.GetSegmentName(subject_name_, 0).SegmentName;

            if(print_on_screen_)
            {
                getConnectionDataInfo();
                std::cout << "Subject: " << subject_name_ << std::endl;
                std::cout << "Segment: " << segment_name_ << "\n" << std::endl;
            }

            // Fill the ViconMsg "robot_position" field
            ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation segment_global_translation;
            getRobotPosition(segment_global_translation);

            writer_vicon_->robot_position[0] = (segment_global_translation.Translation[0])/1000;    // Divided by 1000 to align with ROS topics data
            writer_vicon_->robot_position[1] = (segment_global_translation.Translation[1])/1000;
            writer_vicon_->robot_position[2] = (segment_global_translation.Translation[2])/1000;

            // Fill the ViconMsg "robot_orientation" field
            ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationQuaternion segment_global_rotation_quaternion;
            getRobotOrientation(segment_global_rotation_quaternion);

            writer_vicon_->robot_orientation.x() = segment_global_rotation_quaternion.Rotation[0];
            writer_vicon_->robot_orientation.y() = segment_global_rotation_quaternion.Rotation[1];
            writer_vicon_->robot_orientation.z() = segment_global_rotation_quaternion.Rotation[2];
            writer_vicon_->robot_orientation.w() = segment_global_rotation_quaternion.Rotation[3];

            // Fill the ViconMsg "markers_positions" field
            std::vector<ViconDataStreamSDK::CPP::Output_GetMarkerGlobalTranslation> markers_global_translations{};
            getMarkersPositions(markers_global_translations);

            // EDIT#7/7 use it when PlotJuggler plugin for FastDDS supports sequence data structures
            //for(unsigned int marker_index{0}; marker_index < markers_global_translations.size(); ++marker_index)
            //{
            //    Eigen::Vector3d marker_position{markers_global_translations.at(marker_index).Translation[0],
            //                                    markers_global_translations.at(marker_index).Translation[1],
            //                                    markers_global_translations.at(marker_index).Translation[2]};
            //
            //    writer_vicon_->markers_positions.push_back(marker_position);
            //}

            // Until the changes of "EDIT#" are not applied, the markers positions published are in this form:
            // markers_positions[0] = x coordinate of marker#1
            // markers_positions[1] = y coordinate of marker#1
            // markers_positions[2] = z coordinate of marker#1
            // markers_positions[3] = x coordinate of marker#2
            // and so on...
            for(unsigned int marker_index{0}; marker_index < markers_global_translations.size(); ++marker_index)
            {
                // writer_vicon_->markers_positions.push_back(markers_global_translations.at(marker_index).Translation[0]);
                // writer_vicon_->markers_positions.push_back(markers_global_translations.at(marker_index).Translation[1]);
                // writer_vicon_->markers_positions.push_back(markers_global_translations.at(marker_index).Translation[2]);
                writer_vicon_->markers_positions[marker_index*3] = markers_global_translations[marker_index].Translation[0];
                writer_vicon_->markers_positions[marker_index*3+1] = markers_global_translations[marker_index].Translation[1];
                writer_vicon_->markers_positions[marker_index*3+2] = markers_global_translations[marker_index].Translation[2];
            }

            // Publish the ViconMsg data
            writer_vicon_.publish();

            // Clear the old markers data
            // writer_vicon_->markers_positions.clear();
        }

        void ViconBridge::connectToVicon()
        {
            std::string host_name{"192.168.123.13:801"};

            std::cout << "Connecting to Vicon (" << host_name << ") ..." << std::flush;            

            // Connect to a server
            while (!client_.IsConnected().Connected)
            {
                // Direct connection
                const ViconDataStreamSDK::CPP::Output_Connect connect_result = client_.Connect(host_name);

                if (!(connect_result.Result == ViconDataStreamSDK::CPP::Result::Success))
                {
                    std::cout << "Warning - connect failed... ";
                    switch (connect_result.Result)
                    {
                        case ViconDataStreamSDK::CPP::Result::ClientAlreadyConnected:
                        std::cout << "Client Already Connected" << std::endl;
                        break;
                        case ViconDataStreamSDK::CPP::Result::InvalidHostName:
                        std::cout << "Invalid Host Name" << std::endl;
                        break;
                        case ViconDataStreamSDK::CPP::Result::ClientConnectionFailed:
                        std::cout << "Client Connection Failed" << std::endl;
                        break;
                        default:
                        std::cout << "Unrecognized Error: " << connect_result.Result << std::endl;
                        break;
                    }
                }
                sleep(1);
            }

            std::cout << std::endl;

            getVersion();

            client_.EnableSegmentData();
            client_.EnableMarkerData();

            client_.SetStreamMode(ViconDataStreamSDK::CPP::StreamMode::ServerPush);

            // Set the global up axis
            client_.SetAxisMapping(ViconDataStreamSDK::CPP::Direction::Forward, 
                                   ViconDataStreamSDK::CPP::Direction::Left, 
                                   ViconDataStreamSDK::CPP::Direction::Up); // Z-up
        }

        void ViconBridge::disconnectFromVicon()
        {
            client_.DisableSegmentData();
            client_.DisableMarkerData();

            // Disconnect
            std::cout << " Disconnecting from Vicon ..." << std::endl;
            client_.Disconnect();
        }

        void ViconBridge::getVersion()
        {
            ViconDataStreamSDK::CPP::Output_GetVersion vicon_sdk_version = client_.GetVersion();
            std::cout << "Vicon DataStream SDK version: " << vicon_sdk_version.Major << "."
                                                          << vicon_sdk_version.Minor << "."
                                                          << vicon_sdk_version.Point << "."
                                                          << vicon_sdk_version.Revision << std::endl;
        }

        void ViconBridge::getConnectionDataInfo()
        {
            std::cout << "\n*****************************************************************" << std::endl;

            // Get the frame number
            ViconDataStreamSDK::CPP::Output_GetFrameNumber frame_number = client_.GetFrameNumber();
            std::cout << "Frame Number: " << frame_number.FrameNumber << std::endl;

            // Get the frame rate
            ViconDataStreamSDK::CPP::Output_GetFrameRate frame_rate = client_.GetFrameRate();
            std::cout << "Frame rate: " << frame_rate.FrameRateHz << std::endl;

            for(unsigned int frame_rate_index{0}; frame_rate_index < client_.GetFrameRateCount().Count; ++frame_rate_index)
            {
                std::string frame_rate_name = client_.GetFrameRateName(frame_rate_index).Name;
                double frame_rate_value = client_.GetFrameRateValue(frame_rate_name).Value;

                std::cout << frame_rate_name << ": " << frame_rate_value << "Hz" << std::endl;
            }

            // Get the latency
            std::cout << "Latency: " << client_.GetLatencyTotal().Total << "s" << std::endl;

            for(unsigned int latency_sample_index{0}; latency_sample_index < client_.GetLatencySampleCount().Count; ++latency_sample_index)
            {
                std::string sample_name  = client_.GetLatencySampleName(latency_sample_index).Name;
                double sample_value = client_.GetLatencySampleValue(sample_name).Value;

                std::cout << "  " << sample_name << " " << sample_value << "s" << std::endl;
            }

            std::cout << std::endl;
        }

        void ViconBridge::getRobotPosition(ViconDataStreamSDK::CPP::Output_GetSegmentGlobalTranslation& segment_global_translation)
        {
            // Get the global segment translation
            segment_global_translation = client_.GetSegmentGlobalTranslation(subject_name_, segment_name_);

            if(print_on_screen_)
            {
                std::cout << "Robot position: (" << (segment_global_translation.Translation[0])/1000 << ", "
                                                 << (segment_global_translation.Translation[1])/1000 << ", "
                                                 << (segment_global_translation.Translation[2])/1000 << "), Occluded: "
                                                 << segment_global_translation.Occluded << std::endl;
            }
        }

        void ViconBridge::getRobotOrientation(ViconDataStreamSDK::CPP::Output_GetSegmentGlobalRotationQuaternion& segment_global_rotation_quaternion)
        {
            // Get the global segment rotation in quaternion co-ordinates
            segment_global_rotation_quaternion = client_.GetSegmentGlobalRotationQuaternion(subject_name_, segment_name_);

            if(print_on_screen_)
            {
                std::cout << "Robot orientation: (" << segment_global_rotation_quaternion.Rotation[0] << ", "
                                                    << segment_global_rotation_quaternion.Rotation[1] << ", "
                                                    << segment_global_rotation_quaternion.Rotation[2] << ", "
                                                    << segment_global_rotation_quaternion.Rotation[3] << "), Occluded: "
                                                    << segment_global_rotation_quaternion.Occluded << std::endl;
            }
        }

        void ViconBridge::getMarkersPositions(std::vector<ViconDataStreamSDK::CPP::Output_GetMarkerGlobalTranslation>& markers_global_translations)
        {
            // Count the number of markers
            unsigned int marker_count = client_.GetMarkerCount(subject_name_).MarkerCount;
            for(unsigned int marker_index{0} ; marker_index < marker_count ; ++marker_index)
            {
                // Get the marker name
                std::string marker_name = client_.GetMarkerName(subject_name_, marker_index).MarkerName;

                // Get the global marker translation
                ViconDataStreamSDK::CPP::Output_GetMarkerGlobalTranslation marker_global_translation = client_.GetMarkerGlobalTranslation(subject_name_, marker_name);

                markers_global_translations.push_back(marker_global_translation);

                if(print_on_screen_)
                {
                    std::cout << "Marker " << marker_index + 1 << "/" << marker_count << " - " << marker_name << " ("
                                           << marker_global_translation.Translation[0] << ", "
                                           << marker_global_translation.Translation[1] << ", "
                                           << marker_global_translation.Translation[2] << "), Occluded: "
                                           << marker_global_translation.Occluded << std::endl;
                }
            }
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