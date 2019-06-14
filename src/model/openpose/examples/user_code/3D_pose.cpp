// ------------------------- OpenPose C++ API Tutorial - Example 13 - Custom Input -------------------------
// Synchronous mode: ideal for production integration. It provides the fastest results with respect to runtime
// performance.
// In this function, the user can implement its own way to create frames (e.g., reading his own folder of images).

// Command-line user intraface
#define OPENPOSE_FLAGS_DISABLE_PRODUCER
#include <openpose/flags.hpp>

// OpenPose dependencies
#include <openpose/headers.hpp>

// Pose3D dependencies
#include <mutex>
#include <opencv2/opencv.hpp>
#include <map>
#include <vector>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <time.h>
#include <stdio.h>

// Custom OpenPose flags
// Producer
DEFINE_string(scene_dir, "scenes/test-scene", "Process a directory of a scene. Should have 3DPose format.");

namespace InputReader3D
{
	class InputPose3D
	{
	public:
		InputPose3D(std::string& scenePath);
		void readCaptureInfo();
		void readCameraParameters();
		std::vector<int> getCamerasId();
		cv::Mat getFrame(int camId);
		cv::Mat getIntrinsics(int camId);
		cv::Mat getDistortions(int camId);
		cv::Mat getExtrinsics(int camId);
		int getCurrentFrame();
		void nextFrame();
		bool hasFrames();
	private:
		std::string scenePath;
		int currentFrame;
		int maxFrames;
		std::vector<int> camerasId;
		std::map<int, cv::Mat> extrinsics;
		std::map<int, cv::Mat> intrinsics;
		std::map<int, cv::Mat> distortions;
	};

	InputPose3D::InputPose3D(std::string& scenePath)
	{
		this->scenePath = scenePath;
		this->camerasId = std::vector<int>();
		this->currentFrame = 0;
		readCaptureInfo();

		this->extrinsics = std::map<int, cv::Mat>();
		this->intrinsics = std::map<int, cv::Mat>();
		this->distortions = std::map<int, cv::Mat>();
		readCameraParameters();
	}

	void InputPose3D::readCaptureInfo()
	{
		std::string capturePath = scenePath + "/mocap/capture.json";

		boost::property_tree::ptree root;
		boost::property_tree::read_json(capturePath, root);

		for (boost::property_tree::ptree::value_type& camera : root.get_child("capture.cameras"))
		{
			this->camerasId.push_back(camera.second.get_value<int>());
		}

		this->maxFrames = root.get<int>("capture.frames");
	}

	void InputPose3D::readCameraParameters()
	{
		// Read intrinsics

		std::string intrinsicsPath = scenePath + "/intrinsics.json";

		if (!boost::filesystem::exists(intrinsicsPath))
		{
			intrinsicsPath = scenePath + "/../default.json";
		}

		boost::property_tree::ptree root;
		boost::property_tree::read_json(intrinsicsPath, root);

		for (boost::property_tree::ptree::value_type& cameraNode : root.get_child("calibration.cameras"))
		{
			int cameraNumber = cameraNode.second.get<int>("cameraNumber");

			cv::Mat calibrationMatrix = cv::Mat::zeros(3, 3, CV_64F);
			calibrationMatrix.at<double>(0, 0) = cameraNode.second.get<double>("calibrationMatrix.fx");
			calibrationMatrix.at<double>(1, 1) = cameraNode.second.get<double>("calibrationMatrix.fy");
			calibrationMatrix.at<double>(0, 2) = cameraNode.second.get<double>("calibrationMatrix.cx");
			calibrationMatrix.at<double>(1, 2) = cameraNode.second.get<double>("calibrationMatrix.cy");
			calibrationMatrix.at<double>(2, 2) = 1.0;

			cv::Mat distortionCoefficients = cv::Mat::zeros(1, 5, CV_64F);
			distortionCoefficients.at<double>(0, 0) = cameraNode.second.get<double>("distortionCoefficients.k1");
			distortionCoefficients.at<double>(0, 1) = cameraNode.second.get<double>("distortionCoefficients.k2");
			distortionCoefficients.at<double>(0, 2) = cameraNode.second.get<double>("distortionCoefficients.p1");
			distortionCoefficients.at<double>(0, 3) = cameraNode.second.get<double>("distortionCoefficients.p2");
			distortionCoefficients.at<double>(0, 4) = cameraNode.second.get<double>("distortionCoefficients.k3");

			this->intrinsics[cameraNumber] = calibrationMatrix;
			this->distortions[cameraNumber] = distortionCoefficients;
		}

		// Read extrinsics

		std::string extrinsicsFile = scenePath + "/extrinsics.json";

		boost::property_tree::read_json(extrinsicsFile, root);

		for (boost::property_tree::ptree::value_type& cameraNode : root.get_child("calibration.cameras"))
		{
			int cameraNumber = cameraNode.second.get<int>("cameraNumber");

			cv::Mat translationVector = cv::Mat(3, 1, CV_64F);
			translationVector.at<double>(0, 0) = cameraNode.second.get<double>("translationVector.x");
			translationVector.at<double>(1, 0) = cameraNode.second.get<double>("translationVector.y");
			translationVector.at<double>(2, 0) = cameraNode.second.get<double>("translationVector.z");

			cv::Mat rotationVector = cv::Mat(3, 1, CV_64F);
			rotationVector.at<double>(0, 0) = cameraNode.second.get<double>("rotationVector.x");
			rotationVector.at<double>(1, 0) = cameraNode.second.get<double>("rotationVector.y");
			rotationVector.at<double>(2, 0) = cameraNode.second.get<double>("rotationVector.z");

			cv::Mat rotationMatrix;
			cv::Rodrigues(rotationVector, rotationMatrix);

			cv::Mat mergedMatrix;
			cv::hconcat(rotationMatrix, translationVector, mergedMatrix);

			this->extrinsics[cameraNumber] = mergedMatrix;
		}
	}

	std::vector<int> InputPose3D::getCamerasId()
	{
		return camerasId;
	}

	cv::Mat InputPose3D::getFrame(int camId)
	{
		std::string framePath = scenePath + "/mocap/cam-" + std::to_string(camId) + "/" + std::to_string(currentFrame) + ".png";

		cv::Mat distortedImage = cv::imread(framePath);
		//cv::Mat undistortedImage = cv::Mat(distortedImage.rows, distortedImage.cols, distortedImage.type());
		//cv::undistort(distortedImage, undistortedImage, intrinsics[camId], distortions[camId]);

		return distortedImage;
	}

	cv::Mat InputPose3D::getIntrinsics(int camId)
	{
		return intrinsics[camId];
	}

	cv::Mat InputPose3D::getDistortions(int camId)
	{
		return distortions[camId];
	}

	cv::Mat InputPose3D::getExtrinsics(int camId)
	{
		return extrinsics[camId];
	}

	void InputPose3D::nextFrame()
	{
		currentFrame++;
	}

	bool InputPose3D::hasFrames()
	{
		return currentFrame < maxFrames;
	}

	int InputPose3D::getCurrentFrame()
	{
		return currentFrame;
	}

	class OutputPose3D
	{
	public:
		OutputPose3D(std::string& scenePath);
		void processResults();
	private:
		void prepareJoints();
		std::string getDateString();
		std::map<int, std::pair<cv::Point3d, bool>> getJoints3D(int frameNumber);
		std::vector<std::pair<cv::Point3d, cv::Point3d>> getJointsConnections3D(std::map<int, std::pair<cv::Point3d, bool>> joints);
		std::string scenePath;
		int framesNumber;
		std::vector<std::pair<int, int>> jointsConnections;
	};

	OutputPose3D::OutputPose3D(std::string& scenePath)
	{
		std::string capturePath = scenePath + "/mocap/capture.json";
		boost::property_tree::ptree root;
		boost::property_tree::read_json(capturePath, root);

		this->scenePath = scenePath;
		this->framesNumber = root.get<int>("capture.frames");
		this->jointsConnections = std::vector<std::pair<int, int>>();

		prepareJoints();
	}

	void OutputPose3D::prepareJoints()
	{
		jointsConnections.push_back(std::pair<int, int>(4, 3));
		jointsConnections.push_back(std::pair<int, int>(3, 2));
		jointsConnections.push_back(std::pair<int, int>(2, 1));
		jointsConnections.push_back(std::pair<int, int>(7, 6));
		jointsConnections.push_back(std::pair<int, int>(6, 5));
		jointsConnections.push_back(std::pair<int, int>(5, 1));
		jointsConnections.push_back(std::pair<int, int>(17, 15));
		jointsConnections.push_back(std::pair<int, int>(15, 0));
		jointsConnections.push_back(std::pair<int, int>(18, 16));
		jointsConnections.push_back(std::pair<int, int>(16, 0));
		jointsConnections.push_back(std::pair<int, int>(0, 1));
		jointsConnections.push_back(std::pair<int, int>(1, 8));
		jointsConnections.push_back(std::pair<int, int>(23, 22));
		jointsConnections.push_back(std::pair<int, int>(24, 11));
		jointsConnections.push_back(std::pair<int, int>(22, 11));
		jointsConnections.push_back(std::pair<int, int>(11, 10));
		jointsConnections.push_back(std::pair<int, int>(10, 9));
		jointsConnections.push_back(std::pair<int, int>(9, 8));
		jointsConnections.push_back(std::pair<int, int>(20, 19));
		jointsConnections.push_back(std::pair<int, int>(19, 14));
		jointsConnections.push_back(std::pair<int, int>(21, 14));
		jointsConnections.push_back(std::pair<int, int>(14, 13));
		jointsConnections.push_back(std::pair<int, int>(13, 12));
		jointsConnections.push_back(std::pair<int, int>(12, 8));
	}

	void OutputPose3D::processResults()
	{
		op::log("Starting to write Pose3D results...", op::Priority::High);

		boost::property_tree::ptree root;

		std::string date = getDateString();
		root.put("reconstruction.date", date);

		int cameraNumber = 0;
		boost::property_tree::ptree allFramesNode;
		for (int frameNumber = 0; frameNumber < framesNumber; frameNumber++)
		{
			op::log("Processing frame " + std::to_string(frameNumber) + "...", op::Priority::High);
			std::map<int, std::pair<cv::Point3d, bool>> joints = getJoints3D(frameNumber);

			boost::property_tree::ptree packetDataNode;
			boost::property_tree::ptree packetNode;

			packetNode.put("cameraNumber", cameraNumber);

			boost::property_tree::ptree cameraPointsNode;
			for (std::pair<int, std::pair<cv::Point3d, bool>> jointNode : joints)
			{
					int jointNumber = jointNode.first;
					std::pair<cv::Point3d, bool> jointData = jointNode.second;
					if (jointData.second)
					{
						boost::property_tree::ptree pointNode;
						pointNode.put("x", jointData.first.x);
						pointNode.put("y", jointData.first.y);
						pointNode.put("z", jointData.first.z);
						cameraPointsNode.push_back(make_pair("", pointNode));
					}
			}
			packetNode.add_child("points", cameraPointsNode);

			boost::property_tree::ptree cameraLinesNode;
			for (std::pair<cv::Point3d, cv::Point3d> line : getJointsConnections3D(joints))
			{
				boost::property_tree::ptree lineNode;
				lineNode.put("x1", line.first.x);
				lineNode.put("y1", line.first.y);
				lineNode.put("z1", line.first.z);
				lineNode.put("x2", line.second.x);
				lineNode.put("y2", line.second.y);
				lineNode.put("z2", line.second.z);
				cameraLinesNode.push_back(make_pair("", lineNode));
			}
			packetNode.add_child("lines", cameraLinesNode);
			packetDataNode.push_back(make_pair("", packetNode));

			boost::property_tree::ptree frameNode;
			frameNode.put("frameNumber", frameNumber);
			frameNode.add_child("frameData", packetDataNode);
			allFramesNode.push_back(make_pair("", frameNode));
		}

		root.add_child("reconstruction.frames", allFramesNode);

		std::string reconstructionFile = scenePath + "/reconstruction-mocap.json";
		boost::property_tree::write_json(reconstructionFile, root);

		op::log("Finishd writing Pose3D results...", op::Priority::High);
	}

	std::map<int, std::pair<cv::Point3d, bool>> OutputPose3D::getJoints3D(int frameNumber)
	{
		std::map<int, std::pair<cv::Point3d, bool>> joints3D = std::map<int, std::pair<cv::Point3d, bool>>();

		try
		{
			std::string jointsFile = scenePath + "/results/" + std::to_string(frameNumber) + "_keypoints.json";

			boost::property_tree::ptree root;
			boost::property_tree::read_json(jointsFile, root);

			for (boost::property_tree::ptree::value_type& peopleNode : root.get_child("people"))
			{
				int index = 0;
				int jointIndex = 0;
				double point[] = { 0.0, 0.0, 0.0 };
				for (boost::property_tree::ptree::value_type& keyPointsNode : peopleNode.second.get_child("pose_keypoints_3d"))
				{
					if (index == 3)
					{
						bool jointExists = keyPointsNode.second.get_value<int>() == 1;
						cv::Point3d point3D = cv::Point3d(point[0], point[1], point[2]);
						joints3D[jointIndex] = std::pair<cv::Point3d, bool>(point3D, jointExists);

						index = 0;
						jointIndex++;
						continue;
					}
					else
					{
						point[index] = keyPointsNode.second.get_value<double>();
						index++;
					}
				}
			}

			return joints3D;
		}
		catch (const std::exception & e)
		{
			op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
			return joints3D;
		}
	}

	std::vector<std::pair<cv::Point3d, cv::Point3d>> OutputPose3D::getJointsConnections3D(std::map<int, std::pair<cv::Point3d, bool>> joints)
	{
		std::vector<std::pair<cv::Point3d, cv::Point3d>> jointsConnections3D;
		try
		{
			for (std::pair<int, int> jointConnection : jointsConnections)
			{
				if (joints[jointConnection.first].second && joints[jointConnection.second].second)
				{
					cv::Point3d first = joints[jointConnection.first].first;
					cv::Point3d second = joints[jointConnection.second].first;

					std::pair<cv::Point3d, cv::Point3d> line = std::pair<cv::Point3d, cv::Point3d>(first, second);
					jointsConnections3D.push_back(line);
				}
			}

			return jointsConnections3D;
		}
		catch (const std::exception & e)
		{
			op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
			return jointsConnections3D;
		}
	}

	std::string OutputPose3D::getDateString()
	{
		time_t rawTime = time(NULL);
		char buffer[26];
		ctime_r(&rawTime, buffer);
		std::string date = std::string(buffer);
		date.erase(std::remove(date.begin(), date.end(), '\n'), date.end());

		return date;
	}
}

class WUserInput : public op::WorkerProducer<std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>>>
{
public:
		WUserInput(std::string& scenePath)
		{
			this->mReader = std::make_shared<InputReader3D::InputPose3D>(scenePath);
		}

    void initializationOnThread() {}

    std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>> workProducer()
    {
		try
		{
			if (!mReader->hasFrames())
			{
				std::lock_guard<std::mutex> g(mLock);

				if (mBlocked.empty())
				{
					op::log("No more frames and queue is empty. Closing program.", op::Priority::High);
					this->stop();

					return nullptr;
				}
				else
				{
					auto datumToProcess = mBlocked.front();
					mBlocked.pop();

					std::cout << "Popped => Cam:" << datumToProcess->at(0)->subId << " Frame:" << datumToProcess->at(0)->frameNumber << std::endl;
					return datumToProcess;
				}
			}
			else
			{
				std::lock_guard<std::mutex> g(mLock);

				if (mBlocked.empty())
				{
					op::log("Adding new images of frame " + std::to_string(mReader->getCurrentFrame()) + ".", op::Priority::High);

					std::vector<int> camerasId = mReader->getCamerasId();

					for (int cameraIndex = 0; cameraIndex < camerasId.size(); cameraIndex++)
					{
						int cameraNumber = camerasId[cameraIndex];

						// Create new datum
						auto datumsPtr = std::make_shared<std::vector<std::shared_ptr<op::Datum>>>();
						auto datumPtr = std::make_shared<op::Datum>();

						// Fill datum
						datumPtr->frameNumber = mReader->getCurrentFrame();

						datumPtr->cvInputData = mReader->getFrame(cameraNumber);
						datumPtr->cvOutputData = datumPtr->cvInputData;

						datumPtr->subId = cameraIndex;
						datumPtr->subIdMax = camerasId.size() - 1;

						datumPtr->cameraIntrinsics = mReader->getIntrinsics(cameraNumber);
						datumPtr->cameraExtrinsics = mReader->getExtrinsics(cameraNumber);
						datumPtr->cameraMatrix = datumPtr->cameraIntrinsics * datumPtr->cameraExtrinsics;

						std::cout << "Pushed => Cam:" << cameraNumber << " Frame:" << mReader->getCurrentFrame() << " SubId:" << cameraIndex << " SubIdMax:" << camerasId.size() - 1 << std::endl;
						datumsPtr->push_back(datumPtr);
						mBlocked.push(datumsPtr);
					}

					mReader->nextFrame();
				}

				auto datumToProcess = mBlocked.front();
				mBlocked.pop();

				std::cout << "Popped => Cam:" << datumToProcess->at(0)->subId << " Frame:" << datumToProcess->at(0)->frameNumber << std::endl;
				return datumToProcess;
			}
		}
		catch (const std::exception & e)
		{
			this->stop();
			op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
			return nullptr;
		}
  }

private:
	std::shared_ptr<InputReader3D::InputPose3D> mReader;
	std::queue<std::shared_ptr<std::vector<std::shared_ptr<op::Datum>>>> mBlocked;
	std::mutex mLock;
};

void configureWrapper(op::Wrapper& opWrapper)
{
    try
    {
        // Configuring OpenPose

        // logging_level
        op::check(0 <= FLAGS_logging_level && FLAGS_logging_level <= 255, "Wrong logging_level value.",
                  __LINE__, __FUNCTION__, __FILE__);
        op::ConfigureLog::setPriorityThreshold((op::Priority)FLAGS_logging_level);
        op::Profiler::setDefaultX(FLAGS_profile_speed);

        // Applying user defined configuration - GFlags to program variables
        // outputSize
        const auto outputSize = op::flagsToPoint(FLAGS_output_resolution, "-1x-1");
        // netInputSize
        const auto netInputSize = op::flagsToPoint(FLAGS_net_resolution, "-1x368");
        // faceNetInputSize
        const auto faceNetInputSize = op::flagsToPoint(FLAGS_face_net_resolution, "368x368 (multiples of 16)");
        // handNetInputSize
        const auto handNetInputSize = op::flagsToPoint(FLAGS_hand_net_resolution, "368x368 (multiples of 16)");
        // poseMode
        const auto poseMode = op::flagsToPoseMode(FLAGS_body);
        // poseModel
        const auto poseModel = op::flagsToPoseModel(FLAGS_model_pose);
        // JSON saving
        if (!FLAGS_write_keypoint.empty())
            op::log("Flag `write_keypoint` is deprecated and will eventually be removed."
                    " Please, use `write_json` instead.", op::Priority::Max);
        // keypointScaleMode
        const auto keypointScaleMode = op::flagsToScaleMode(FLAGS_keypoint_scale);
        // heatmaps to add
        const auto heatMapTypes = op::flagsToHeatMaps(FLAGS_heatmaps_add_parts, FLAGS_heatmaps_add_bkg,
                                                      FLAGS_heatmaps_add_PAFs);
        const auto heatMapScaleMode = op::flagsToHeatMapScaleMode(FLAGS_heatmaps_scale);
        // >1 camera view?
        const auto multipleView = (FLAGS_3d || FLAGS_3d_views > 1);
        //const auto multipleView = false;
        // Face and hand detectors
        const auto faceDetector = op::flagsToDetector(FLAGS_face_detector);
        const auto handDetector = op::flagsToDetector(FLAGS_hand_detector);
        // Enabling Google Logging
        const bool enableGoogleLogging = true;

        // Initializing the user custom classes
        // Frames producer (e.g., video, webcam, ...)
        auto wUserInput = std::make_shared<WUserInput>(FLAGS_scene_dir);
        // Add custom processing
        const auto workerInputOnNewThread = true;
        opWrapper.setWorker(op::WorkerType::Input, wUserInput, workerInputOnNewThread);

        // Pose configuration (use WrapperStructPose{} for default and recommended configuration)
        const op::WrapperStructPose wrapperStructPose{
            poseMode, netInputSize, outputSize, keypointScaleMode, FLAGS_num_gpu, FLAGS_num_gpu_start,
            FLAGS_scale_number, (float)FLAGS_scale_gap, op::flagsToRenderMode(FLAGS_render_pose, multipleView),
            poseModel, !FLAGS_disable_blending, (float)FLAGS_alpha_pose, (float)FLAGS_alpha_heatmap,
            FLAGS_part_to_show, FLAGS_model_folder, heatMapTypes, heatMapScaleMode, FLAGS_part_candidates,
            (float)FLAGS_render_threshold, FLAGS_number_people_max, FLAGS_maximize_positives, FLAGS_fps_max,
            FLAGS_prototxt_path, FLAGS_caffemodel_path, (float)FLAGS_upsampling_ratio, enableGoogleLogging};
        opWrapper.configure(wrapperStructPose);
        // Face configuration (use op::WrapperStructFace{} to disable it)
        const op::WrapperStructFace wrapperStructFace{
            FLAGS_face, faceDetector, faceNetInputSize,
            op::flagsToRenderMode(FLAGS_face_render, multipleView, FLAGS_render_pose),
            (float)FLAGS_face_alpha_pose, (float)FLAGS_face_alpha_heatmap, (float)FLAGS_face_render_threshold};
        opWrapper.configure(wrapperStructFace);
        // Hand configuration (use op::WrapperStructHand{} to disable it)
        const op::WrapperStructHand wrapperStructHand{
            FLAGS_hand, handDetector, handNetInputSize, FLAGS_hand_scale_number, (float)FLAGS_hand_scale_range,
            op::flagsToRenderMode(FLAGS_hand_render, multipleView, FLAGS_render_pose), (float)FLAGS_hand_alpha_pose,
            (float)FLAGS_hand_alpha_heatmap, (float)FLAGS_hand_render_threshold};
        opWrapper.configure(wrapperStructHand);
        // Extra functionality configuration (use op::WrapperStructExtra{} to disable it)
        const op::WrapperStructExtra wrapperStructExtra{
            FLAGS_3d, FLAGS_3d_min_views, FLAGS_identification, FLAGS_tracking, FLAGS_ik_threads};
        opWrapper.configure(wrapperStructExtra);
        // Output (comment or use default argument to disable any output)
        const op::WrapperStructOutput wrapperStructOutput{
            FLAGS_cli_verbose, FLAGS_write_keypoint, op::stringToDataFormat(FLAGS_write_keypoint_format),
            FLAGS_write_json, FLAGS_write_coco_json, FLAGS_write_coco_json_variants, FLAGS_write_coco_json_variant,
            FLAGS_write_images, FLAGS_write_images_format, FLAGS_write_video, FLAGS_write_video_fps,
            FLAGS_write_video_with_audio, FLAGS_write_heatmaps, FLAGS_write_heatmaps_format, FLAGS_write_video_3d,
            FLAGS_write_video_adam, FLAGS_write_bvh, FLAGS_udp_host, FLAGS_udp_port};
        opWrapper.configure(wrapperStructOutput);
        // GUI (comment or use default argument to disable any visual output)
        const op::WrapperStructGui wrapperStructGui{
            op::flagsToDisplayMode(FLAGS_display, FLAGS_3d), !FLAGS_no_gui_verbose, FLAGS_fullscreen};
        opWrapper.configure(wrapperStructGui);
        // Set to single-thread (for sequential processing and/or debugging and/or reducing latency)
        if (FLAGS_disable_multi_thread)
            opWrapper.disableMultiThreading();
    }
    catch (const std::exception& e)
    {
        op::error(e.what(), __LINE__, __FUNCTION__, __FILE__);
    }
}

int tutorialApiCpp()
{
    try
    {
        op::log("Starting OpenPose demo...", op::Priority::High);
        const auto opTimer = op::getTimerInit();

        // OpenPose wrapper
        op::log("Configuring OpenPose...", op::Priority::High);
        op::Wrapper opWrapper;
        configureWrapper(opWrapper);

        // Start, run, and stop processing - exec() blocks this thread until OpenPose wrapper has finished
        op::log("Starting thread(s)...", op::Priority::High);
        opWrapper.exec();

        // Measuring total time
        op::printTime(opTimer, "OpenPose demo successfully finished. Total time: ", " seconds.", op::Priority::High);

				// Dumping results in Pose3D format
				InputReader3D::OutputPose3D outputWriter(FLAGS_scene_dir);
				outputWriter.processResults();

        // Return
        return 0;
    }
    catch (const std::exception& e)
    {
        return -1;
    }
}

int main(int argc, char *argv[])
{
    // Parsing command line flags
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    // Running tutorialApiCpp
    return tutorialApiCpp();
}
