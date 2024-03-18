#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;

int main() {
    // Read calibration parameters from file
    ifstream inFile("calibration_params.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file" << endl;
        return -1;
    }

    cv::Mat cameraMatrix(3, 3, CV_64F);
    cv::Mat distCoeffs(5, 1, CV_64F);
    double rms;

    string line;
    while (getline(inFile, line)) {
        if (line == "Camera matrix:") {
            for (int i = 0; i < cameraMatrix.rows; ++i) {
                for (int j = 0; j < cameraMatrix.cols; ++j) {
                    inFile >> cameraMatrix.at<double>(i, j);
                }
            }
        } 
        if (line == "Distortion coefficients:") {
            for (int i = 0; i < distCoeffs.rows; ++i) {
                inFile >> distCoeffs.at<double>(i);
            }
        } 
        if (line.find("Reprojection error:") != string::npos) {
            inFile >> rms;
        }
    }

    // Close file
    inFile.close();

    // Print loaded parameters
    cout << "Loaded Camera matrix: \n" << cameraMatrix << endl;
    cout << "Loaded Distortion coefficients: \n" << distCoeffs << endl;
    cout << "Loaded Reprojection error: " << rms << endl;

    // The dimensions of the chessboard
    cv::Size chessboardSize(9, 6);
    float squareSize = 1.0; // Assuming each square of the chessboard is 1 unit in size

    // Generate object points for chessboard corners
    vector<cv::Point3f> objectPoints;
    for (int y = 0; y < chessboardSize.height; ++y) {
        for (int x = 0; x < chessboardSize.width; ++x) {
            objectPoints.push_back(cv::Point3f(x * squareSize, y * squareSize, 0));
        }
    }

    // Define the axes points for visualization
    vector<cv::Point3f> axesPoints;
    axesPoints.push_back(cv::Point3f(0, 0, 0));  // origin
    axesPoints.push_back(cv::Point3f(squareSize, 0, 0));  // x-axis end point
    axesPoints.push_back(cv::Point3f(0, squareSize, 0));  // y-axis end point
    axesPoints.push_back(cv::Point3f(0, 0, -squareSize));  // z-axis end point

    // Calculate teh center of the chessbaord
    float boardCenterX = (chessboardSize.width - 1) * squareSize / 2.0;
    float boardCenterY = (chessboardSize.height - 1) * squareSize / 2.0;

    //Define the vitual objects points for visualization
    vector<cv::Point3f> virtualObjectPoints;
    virtualObjectPoints.push_back(cv::Point3f(boardCenterX,boardCenterY,2)); // Apex of the pyramid
    virtualObjectPoints.push_back(cv::Point3f(boardCenterX + 1,boardCenterY + 1,0)); // Base point 1
    virtualObjectPoints.push_back(cv::Point3f(boardCenterX - 1,boardCenterY + 1,0)); // Base point 2
    virtualObjectPoints.push_back(cv::Point3f(boardCenterX - 1,boardCenterY - 1,0)); // Base point 3
    virtualObjectPoints.push_back(cv::Point3f(boardCenterX + 1,boardCenterY - 1,0)); // Base point 4

    // Video capture
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Unable to open camera" << endl;
        return -1;
    }

    while (true) {
        // Capturing the frames
        cv::Mat frame;
        cap >> frame;

        // Convert frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Find chessboard corners
        vector<cv::Point2f> corners;
        bool patternFound = cv::findChessboardCorners(gray, chessboardSize, corners);

        if (patternFound) {
            // Refine corner locations
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));

            // Draw corners on the image
            cv::drawChessboardCorners(frame, chessboardSize, cv::Mat(corners), patternFound);

            // Estimate pose using solvePnP
            cv::Mat rvec, tvec;
            cv::solvePnP(objectPoints, corners, cameraMatrix, distCoeffs, rvec, tvec);

            // Project 3D points to image plane
            vector<cv::Point2f> projectedPoints;
            cv::projectPoints(axesPoints, rvec, tvec, cameraMatrix, distCoeffs, projectedPoints);

            // Draw 3D axes
            cv::line(frame, projectedPoints[0], projectedPoints[1], cv::Scalar(0, 0, 255), 2);  // x-axis (red)
            cv::line(frame, projectedPoints[0], projectedPoints[2], cv::Scalar(0, 255, 0), 2);  // y-axis (green)
            cv::line(frame, projectedPoints[0], projectedPoints[3], cv::Scalar(255, 0, 0), 2);  // z-axis (blue)

            // Project 3D points to image plane for virtual object visualization
            vector<cv::Point2f> projectedVirtualObjectPoints;
            cv::projectPoints(virtualObjectPoints, rvec, tvec, cameraMatrix, distCoeffs, projectedVirtualObjectPoints);

            // Draw lines connecting projected points to form the virtual object (pyramid)
            cv::line(frame, projectedVirtualObjectPoints[0], projectedVirtualObjectPoints[1], cv::Scalar(0, 0, 255), 2);  // Line 1
            cv::line(frame, projectedVirtualObjectPoints[0], projectedVirtualObjectPoints[2], cv::Scalar(0, 0, 255), 2);  // Line 2
            cv::line(frame, projectedVirtualObjectPoints[0], projectedVirtualObjectPoints[3], cv::Scalar(0, 0, 255), 2);  // Line 3
            cv::line(frame, projectedVirtualObjectPoints[0], projectedVirtualObjectPoints[4], cv::Scalar(0, 0, 255), 2);  // Line 4
            cv::line(frame, projectedVirtualObjectPoints[1], projectedVirtualObjectPoints[2], cv::Scalar(0, 0, 255), 2);  // Line 5
            cv::line(frame, projectedVirtualObjectPoints[2], projectedVirtualObjectPoints[3], cv::Scalar(0, 0, 255), 2);  // Line 6
            cv::line(frame, projectedVirtualObjectPoints[3], projectedVirtualObjectPoints[4], cv::Scalar(0, 0, 255), 2);  // Line 7
            cv::line(frame, projectedVirtualObjectPoints[4], projectedVirtualObjectPoints[1], cv::Scalar(0, 0, 255), 2);  // Line 8

            // Print rotation and translation vectors
            cout << "Rotation vector: " << rvec << endl;
            cout << "Translation vector: " << tvec << endl;
        }

        cv::imshow("Frame", frame);

        // Break the loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    return 0;
}
