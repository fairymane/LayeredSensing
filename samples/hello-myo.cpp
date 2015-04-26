// Copyright (C) 2013-2014 Thalmic Labs Inc.
// Distributed under the Myo SDK license agreement. See LICENSE.txt for details.
#define _USE_MATH_DEFINES
#include <cmath>
#include <array>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <time.h>

// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

// Classes that inherit from myo::DeviceListener can be used to receive events from Myo devices. DeviceListener
// provides several virtual functions for handling different kinds of events. If you do not override an event, the
// default behavior is to do nothing.
class DataCollector : public myo::DeviceListener {
public:
    DataCollector()
    : onArm(false), isUnlocked(false), roll_w(0), pitch_w(0), yaw_w(0),accel_x(0), accel_y(0), accel_z(0),
		gyro_x(0), gyro_y(0), gyro_z(0), rssi_(0), emg_mili(0), imu_mili(0), emg_timestamp(0), imu_timestamp(0),
		pose_string("unknown"), emgSamples(), currentPose()
    {
		memset(imu_time, 0, sizeof(char) * 30);
		memset(emg_time, 0, sizeof(char) * 30);
    }

    // onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
    void onUnpair(myo::Myo* myo, uint64_t timestamp)
    {
        // We've lost a Myo.
        // Let's clean up some leftover state.
        roll_w = 0;
        pitch_w = 0;
        yaw_w = 0;
		accel_x = 0;
		accel_y = 0;
		accel_z = 0;
		gyro_x = 0;
		gyro_y = 0;
		gyro_z = 0;
		rssi_ = 0;
		imu_timestamp = 0;
		emg_timestamp = 0;
		emg_mili = 0;
		imu_mili = 0;
		memset(imu_time, 0, sizeof(char) * 30) ;
		memset(emg_time, 0, sizeof(char) * 30);
		pose_string = "unknown";
		emgSamples.fill(0);
        onArm = false;
        isUnlocked = false;
    }

    // onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
    // as a unit quaternion.
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
    {
        using std::atan2;
        using std::asin;
        using std::sqrt;
        using std::max;
        using std::min;

        // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
        float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                           1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
        float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
        float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                        1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

        // Convert the floating point angles in radians to a scale from 0 to 18.
        roll_w = static_cast<int>((roll + (float)M_PI)/(M_PI * 2.0f) * 18);
        pitch_w = static_cast<int>((pitch + (float)M_PI/2.0f)/M_PI * 18);
        yaw_w = static_cast<int>((yaw + (float)M_PI)/(M_PI * 2.0f) * 18);
    }

    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
    {
        currentPose = pose;
		pose_string = currentPose.toString();

        if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
            // Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
            // Myo becoming locked.
            myo->unlock(myo::Myo::unlockHold);

            // Notify the Myo that the pose has resulted in an action, in this case changing
            // the text on the screen. The Myo will vibrate.
            myo->notifyUserAction();
        } else {
            // Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
            // are being performed, but lock after inactivity.
            myo->unlock(myo::Myo::unlockTimed);
        }
    }

    // onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
    {
        onArm = true;
        whichArm = arm;
    }

    // onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp)
    {
        onArm = false;
    }

    // onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
    void onUnlock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = true;
    }

    // onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
    void onLock(myo::Myo* myo, uint64_t timestamp)
    {
        isUnlocked = false;
    }

	// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
	void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
	{
		emg_timestamp = timestamp/1000;

		if (emg_timestamp != 0)
		{
			time_t tmp_time = emg_timestamp / 1000;
			emg_mili = emg_timestamp % tmp_time;

			struct tm * timeinfo;
			timeinfo = localtime(&tmp_time);

			strftime(emg_time, sizeof(emg_time), "%m-%d %H:%M:%S:", timeinfo);

			for (int i = 0; i < 8; i++)
			{
				emgSamples[i] = emg[i];
			}			
		}
	}

	// get accelrometer data
	void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel)
	{
        //std::cout <<"timestamp: " <<timestamp <<std::endl;
		imu_timestamp = timestamp/1000;

		time_t tmp_time = imu_timestamp / 1000;

		imu_mili = imu_timestamp % tmp_time;
		struct tm * timeinfo;
		timeinfo = localtime(&tmp_time);

		strftime(imu_time, sizeof(imu_time), "%m-%d %H:%M:%S:", timeinfo);

		accel_x = accel.x();
		accel_y = accel.y();
		accel_z = accel.z();
	}

	// get gyroscope data
	void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro)
	{
		gyro_x = gyro.x();
		gyro_y = gyro.y();
		gyro_z = gyro.z();
	}

	//get RSSI value.
	void onRssi(myo::Myo* myo, uint64_t timestamp, int8_t rssi) 
	{
		rssi_ = rssi;
	}


    // There are other virtual functions in DeviceListener that we could override here, like onAccelerometerData().
    // For this example, the functions overridden above are sufficient.

    // We define this function to print the current values that were updated by the on...() functions above.


    void print()
    {
        // Clear the current line
        std::cout << '\r';

        // Print out the orientation. Orientation data is always available, even if no arm is currently recognized.
        std::cout << '[' << std::string(roll_w, '*') << std::string(18 - roll_w, ' ') << ']'
                  << '[' << std::string(pitch_w, '*') << std::string(18 - pitch_w, ' ') << ']'
                  << '[' << std::string(yaw_w, '*') << std::string(18 - yaw_w, ' ') << ']'<<std::endl;

		//std::cout << "accel_x" << accel_x << "accel_y" << accel_y << "accel_z" << accel_z << std::endl;
		//std::cout << "gyro_x" << gyro_x << "gyro_y" << gyro_y << "gyro_z" << gyro_z << std::endl;

        if (onArm) {
            // Print out the lock state, the currently recognized pose, and which arm Myo is being worn on.

            // Pose::toString() provides the human-readable name of a pose. We can also output a Pose directly to an
            // output stream (e.g. std::cout << currentPose;). In this case we want to get the pose name's length so
            // that we can fill the rest of the field with spaces below, so we obtain it as a string using toString().
            std::string poseString = currentPose.toString();

            std::cout << '[' << (isUnlocked ? "unlocked" : "locked  ") << ']'
                      << '[' << (whichArm == myo::armLeft ? "L" : "R") << ']'
                      << '[' << poseString << std::string(14 - poseString.size(), ' ') << ']';
        } else {
            // Print out a placeholder for the arm and pose when Myo doesn't currently know which arm it's on.
            std::cout << '[' << std::string(8, ' ') << ']' << "[?]" << '[' << std::string(14, ' ') << ']';
        }



        std::cout << std::flush;
    }



	void write_to_file(std::ofstream& ss1, std::ofstream& ss2)
	{
		ss1 <<imu_timestamp <<"," <<std::string(imu_time) + std::to_string(imu_mili) <<"," << accel_x << "," << accel_y << "," << accel_z << "," << gyro_x << "," << gyro_y << "," << gyro_z <<","
			<< roll_w << "," << pitch_w << "," << yaw_w << ","<< (int)rssi_ <<"," << pose_string << "\n";
		ss1.flush();

		if (emg_timestamp != 0)
		{
			std::string emgString;
			ss2 << emg_timestamp << "," << std::string(emg_time) + std::to_string(emg_mili) << ",";
			
			for (size_t i = 0; i < emgSamples.size(); i++)
			{
				std::ostringstream oss;
				oss << static_cast<int>(emgSamples[i]);
				emgString = oss.str();
				ss2 << emgString;
				if(i == emgSamples.size() -1)
					ss2 << "\n";
				else
					ss2 << ",";
			}
			ss2.flush();
		}
	}



    // These values are set by onArmSync() and onArmUnsync() above.
    bool onArm;
    myo::Arm whichArm;

    // This is set by onUnlocked() and onLocked() above.
    bool isUnlocked;

    // These values are set by onOrientationData() and onPose() above.
    int roll_w, pitch_w, yaw_w;
	int8_t rssi_;
	float accel_x, accel_y, accel_z;
	float gyro_x, gyro_y, gyro_z;
	float rotation_x, rotation_y, rotation_z, rotation_w;
	char emg_time[30],  imu_time[30];

	uint64_t emg_timestamp, imu_timestamp;
	int emg_mili, imu_mili;
	std::string pose_string;
	std::array<int8_t, 8> emgSamples;
    myo::Pose currentPose;
};

int main(int argc, char** argv)
{
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {

    // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
    // publishing your application. The Hub provides access to one or more Myos.
    myo::Hub hub("com.example.hello-myo");

    std::cout << "Attempting to find a Myo..." << std::endl;

    // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
    // immediately.
    // waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
    // if that fails, the function will return a null pointer.
    myo::Myo* myo = hub.waitForMyo(10000);

    // If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
    if (!myo) {
        throw std::runtime_error("Unable to find a Myo!");
    }

    // We've found a Myo.
    std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

	// Next we enable EMG streaming on the found Myo.
	myo->setStreamEmg(myo::Myo::streamEmgEnabled);


    // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
    DataCollector collector;

	// Create a output file
	const char* path1 = "/Users/taofeng/Dropbox/ece498-hw2/sdk/output/imu_data.csv ";
	std::ofstream imu(path1);
	const char* path2 = "/Users/taofeng/Dropbox/ece498-hw2/sdk/output/emg_data.csv ";
	std::ofstream emg(path2);

	imu << "timestamp,formatted_time,accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,roll_w,pitch_w,yaw_w,rssi,pose \n";
	imu.flush();

	emg << "timestamp,formatted_time,emg1,emg2,emg3,emg4,emg5,emg6,emg7,emg8" << std::endl;
	emg.flush();

    // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
    // Hub::run() to send events to all registered device listeners.
    hub.addListener(&collector);

    // Finally we enter our main loop.
    while (1) {
        // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
        // In this case, we wish to update our display 20 times a second, so we run for 1000/20 milliseconds.
        hub.run(1000/20);
        // After processing events, we call the print() member function we defined above to print out the values we've
        // obtained from any events that have occurred.
        collector.print();
		collector.write_to_file(imu, emg);
    }
	imu.close();
	emg.close();

    // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }
}
