/*
    Saugat Malla
    Project 4
*/

/*
    Code for extension 1
*/

#include <iostream> // Header file for input and output operations
#include <fstream> // Header file for file stream operations
#include <opencv2/opencv.hpp> // OpenCV header file

using namespace std;

// Function to draw a cube on the image
void drawCube(cv::Mat& image, const vector<cv::Point2f>& corners, const cv::Mat& rvec, const cv::Mat& tvec,
              const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs) {
    // Define cube vertices in 3D space
    vector<cv::Point3f> cubePoints;
    cubePoints.push_back(cv::Point3f(0, 0, 0));             // Bottom front left
    cubePoints.push_back(cv::Point3f(1, 0, 0));             // Bottom front right
    cubePoints.push_back(cv::Point3f(1, 1, 0));             // Bottom back right
    cubePoints.push_back(cv::Point3f(0, 1, 0));             // Bottom back left
    cubePoints.push_back(cv::Point3f(0, 0, -1));            // Top front left
    cubePoints.push_back(cv::Point3f(1, 0, -1));            // Top front right
    cubePoints.push_back(cv::Point3f(1, 1, -1));            // Top back right
    cubePoints.push_back(cv::Point3f(0, 1, -1));            // Top back left

    // Project cube vertices onto image plane
    vector<cv::Point2f> projectedCubePoints;
    cv::projectPoints(cubePoints, rvec, tvec, cameraMatrix, distCoeffs, projectedCubePoints);

    // Draw lines to form the cube
    cv::line(image, projectedCubePoints[0], projectedCubePoints[1], cv::Scalar(0, 0, 255), 2);      // Front bottom
    cv::line(image, projectedCubePoints[1], projectedCubePoints[2], cv::Scalar(0, 0, 255), 2);      // Front right
    cv::line(image, projectedCubePoints[2], projectedCubePoints[3], cv::Scalar(0, 0, 255), 2);      // Back bottom
    cv::line(image, projectedCubePoints[3], projectedCubePoints[0], cv::Scalar(0, 0, 255), 2);      // Left bottom

    cv::line(image, projectedCubePoints[4], projectedCubePoints[5], cv::Scalar(0, 0, 255), 2);      // Front top
    cv::line(image, projectedCubePoints[5], projectedCubePoints[6], cv::Scalar(0, 0, 255), 2);      // Front right
    cv::line(image, projectedCubePoints[6], projectedCubePoints[7], cv::Scalar(0, 0, 255), 2);      // Back top
    cv::line(image, projectedCubePoints[7], projectedCubePoints[4], cv::Scalar(0, 0, 255), 2);      // Left top

    cv::line(image, projectedCubePoints[0], projectedCubePoints[4], cv::Scalar(0, 0, 255), 2);      // Left front bottom
    cv::line(image, projectedCubePoints[1], projectedCubePoints[5], cv::Scalar(0, 0, 255), 2);      // Right front bottom
    cv::line(image, projectedCubePoints[2], projectedCubePoints[6], cv::Scalar(0, 0, 255), 2);      // Right back bottom
    cv::line(image, projectedCubePoints[3], projectedCubePoints[7], cv::Scalar(0, 0, 255), 2);      // Left back bottom
}

int main() {
    // Read calibration parameters from file
    ifstream inFile("calibration_params.txt");
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open file" << endl; // Display error message
        return -1; // Return error code
    }

    cv::Mat cameraMatrix(3, 3, CV_64F); // Initialize camera matrix
    cv::Mat distCoeffs(5, 1, CV_64F); // Initialize distortion coefficients
    double rms; // Initialize reprojection error

    string line;
    while (getline(inFile, line)) { // Read each line from the file
        if (line == "Camera matrix:") { // If line contains camera matrix data
            for (int i = 0; i < cameraMatrix.rows; ++i) {
                for (int j = 0; j < cameraMatrix.cols; ++j) {
                    inFile >> cameraMatrix.at<double>(i, j); // Read camera matrix values
                }
            }
        } 
        if (line == "Distortion coefficients:") { // If line contains distortion coefficients data
            for (int i = 0; i < distCoeffs.rows; ++i) {
                inFile >> distCoeffs.at<double>(i); // Read distortion coefficients values
            }
        } 
        if (line.find("Reprojection error:") != string::npos) { // If line contains reprojection error data
            inFile >> rms; // Read reprojection error value
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
            objectPoints.push_back(cv::Point3f(x * squareSize, y * squareSize, 0)); // Create object points
        }
    }

    // Video capture
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) { // If camera is not opened
        cerr << "Error: Unable to open camera" << endl; // Display error message
        return -1; // Return error code
    }

    while (true) { // Infinite loop
        // Capturing the frames
        cv::Mat frame;
        cap >> frame; // Capture frame from camera

        // Convert frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        // Find chessboard corners
        vector<cv::Point2f> corners;
        bool patternFound = cv::findChessboardCorners(gray, chessboardSize, corners); // Find corners

        if (patternFound) { // If chessboard pattern found
            // Refine corner locations
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001));

            // Draw corners on the image
            cv::drawChessboardCorners(frame, chessboardSize, cv::Mat(corners), patternFound); // Draw corners

            // Estimate pose using solvePnP
            cv::Mat rvec, tvec;
            cv::solvePnP(objectPoints, corners, cameraMatrix, distCoeffs, rvec, tvec); // Estimate pose

            // Draw cube
            drawCube(frame, corners, rvec, tvec, cameraMatrix, distCoeffs); // Draw cube on frame
        }

        cv::imshow("Frame", frame); // Display frame

        // Break the loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') { // If 'q' key is pressed
            break; // Exit the loop
        }
    }

    cap.release(); // Release the camera
    cv::destroyAllWindows(); // Close all OpenCV windows

    return 0; // Return success
}
