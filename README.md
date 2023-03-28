# Vicon Bridge

This repository contains the code needed to interface with the Vicon Nexus software from DLS2 and to get robot pose and robot markers positions, using the Vicon SDK without ROS.\
This version is implemented in the *master* and *develop* branches, while you can use the ROS version from the *ros* branch.

The main files that you can find in the repository are:
* **vicon_bridge** (*include*/*src*)\
It implements the code to interface with Vicon, get its data, publish them using Fast DDS-Gen and print them on the terminal for debug.

* **ViconDataStreamSDK_CPPTest** (*third_party*)\
It contains examples on how to get information from Vicon Nexus and it comes with the official [Vicon DataStream SDK](https://www.vicon.com/software/datastream-sdk/) (version 1.11.0 for Linux64), that contains also the *DataStreamClient.h* file needed to use the Vicon library and all the other dependencies of the *third_party* folder.\
An executable for this file has been created ("vicon_test") and the file has been changed (some parts have been commented) to be able to run it.

## How to execute the code

### Launch vicon_bridge
To launch *vicon_bridge*, you can just compile the "vicon_bridge" DLS2 submodule with ``ccmake ..``, then build and run the ``estimation`` layer and the *vicon_bridge* estimator with:
```
dls -lconsole -lestimation
> loadEstimator vicon_bridge
```
Be sure to be already connected to the Vicon (IP: ``192.168.123.13``) and that you can ping it from inside the DLS2 docker image.

If you are connected to the Vicon (see the [Vicon Wiki](https://gitlab.advr.iit.it/dls-lab/new-wiki/-/wikis/hardware/vicon/vicon)) and see the object with markers correctly constructed in Vicon Nexus (you don't see just the subject grey markers, but these are connected and coloured in yellow) you can get the values from Vicon and see them with Plotjuggler at the *vicon_bridge* topic under "Domain 3".\
**Important:** Remember to enable the publishing of the robot pose from the Vicon (check the Vicon Wiki linked above).

If you enable the *print_on_screen_* variable in "vicon_bridge.hpp", you can also print data for debug, like:
* Vicon DataStream SDK version
* robot pose
* markers positions
* connection information (frame number, frame rate, latency)

The values published on the *vicon_bridge* topic instead, are just:
* timestamp
* robot pose
* markers positions

### Launch vicon_test
You can also launch the *vicon_test* executable for the ViconDataStreamSDK_CPPTest.cpp, to just see what you are getting from the Vicon for debug, with:

```
cd dls2_deploy/build/state_estimator/vicon_bridge/third_party/
./vicon_test
```

You can publish more information in *vicon_bridge* if you want, using the code of the the ViconDataStreamSDK_CPPTest.cpp file. For example, in vicon_bridge we consider to get information from a single subject, with a single segment (e.g. "base"). But if you want to iterate over multiple subjects for example (e.g. you have more robots at the same time working with Vicon), you can do it by following the code written in this file.

### Compare data with ROS topics
If you want to compare the data you get here with the ones published via ROS topics, you can open a second docker image (so, having DLS2 and DLS1 docker images running in parallel) with **-container_name** (to be sure to not close the DLS2 image already open) like:\
``dls-docker.py --api run2 -container_name dls1 -f -nv -e DLS=1 -j dls -i dls-dev``

First, check if you ping the Vicon IP ``192.168.123.13`` from inside DLS1. Then, clone the *vicon_bridge* repository with:
```
cd dls_ws/src
git clone git@gitlab.advr.iit.it:dls-lab/vicon_bridge.git -b ros
```
Change the "datastream_hostport" field of the *vicon.launch* file with the vicon IP ``192.168.123.13:801`` instead of using the alias "vicon", build and launch the code with:\
```
cd dls_ws/src
catkin_make -j$(nproc) install
source devel/setup.bash
roslaunch vicon_bridge vicon.launch
```
Attach a terminal to the DLS1 image with:\
``dls-docker.py attach dls1``

and run:
```
source devel/setup.bash
rostopic echo /vicon/markers
rostopic echo vicon/<subject_name>/<segment_name>
```
You can then check the markers positions by looking at the first topic and the robot pose by looking at the second one.