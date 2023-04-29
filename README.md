# VehicleVisualization

VehicleVisualization is a C++ application designed to provide drivers with an intuitive and user-friendly interface for interpreting messages from Intelligent Transport Systems (ITS) and visualizing traffic. This includes information about surrounding vehicles and infrastructure, hazard warnings, efficient intersection clearance, and more. The application is also useful for verifying the functionality of ITS systems.

The application has been developed for both offline and online operation, allowing it to display ITS messages and events while driving in the vehicle (online mode), as well as playback messages from a file (offline mode). In online mode, the messages provide real-time visualization, enabling drivers to quickly and easily understand the content of the messages and take appropriate action if necessary. The messages that the application can parse and process are defined by the ETSI standard.

The app currently supports the following ITS services: 
- Cooperative Awareness (CA)
- Road Lane Topology (RLT)
- Traffic Light Maneuver (TLM)
- Decentralized Environmental Notification (DEN)

It can parse the following messages: 
- Cooperative Awareness Messages (CAM)
- Decentralized Environmental Notification Messages (DENM)
- Map Data Extended Message (MAPEM)
- Signal Phase and Timing Messages (SPATEM)
- Signal Request Extended Message (SREM)
- GEONetworking headers.

# Dependencies
To build and run the app on Linux, you will need:

- C++11 compiler (tested with GCC 11.3.0)
- QT framework 5.15 
- GPSD service (tested with GPSD 3.22)
- TShark (tested with TShark 3.6.2)

# Installation

To install the necessary packages for QT framework on Debian 11 / Ubuntu 20.10, use the command:
```
sudo apt-get install qt5-default
```
or for higher versions of Debian or Ubuntu, use the command:
```
sudo apt-get install qtbase5-dev
```

To install the remaining dependencies, run:
```
sudo apt-get install build-essential 
sudo apt-get install libgps-dev 
sudo apt-get install gpsd 
sudo apt-get install tshark
```

# Building the app
To build the app, follow these steps:

- Clone the repository:
```
git clone https://github.com/krivmi/VehicleVisualization.git
```

- Change into the project directory:
```
cd VehicleVisualization
```

- Build the app - create a build directory and change into the directory, run qmake .. and make:
```
mkdir build
cd build
qmake ..
make
```

Try to run the application. **It might be necessary** to manually include the QMapControl library in the system. The library is located in /build/src/QMapControl/lib/. To include the library, obtain its full path in the system (either by running "sudo find / -name libqmapcontrol.so" or by manually writing the path) and execute the following command, replacing "/path/to/library" with the actual path to the library:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/library
```
# Running the app
To run the app, execute the following command in the project directory:

```
./build/apps/VehicleVisualization/bin/VehicleVisualization
```

# Testing
To test the application, I have included some sample test files in the apps/resources/trafficFiles folder. After starting the application, you can load a file by selecting "File -> Load traffic file" from the menu. Once the file is uploaded, you can play its messages.

# License
This project is licensed under the GNU General Public License. See the LICENSE file for details.
