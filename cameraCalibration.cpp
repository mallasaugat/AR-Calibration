/*
    Saugat Malla
    Project 4
*/

/*
    Code for taks 1-3 
*/


#include<iostream> // Header file for input and output operations
#include<opencv2/opencv.hpp> // OpenCV header file
#include<vector> // Header file for vector container
#include<fstream> // Header file for file stream operations

using namespace std;

int main(){

    // The dimensions of the chessboard (inner corners)
    cv::Size chessboardSize(9,6);

    // Video Capture
    cv::VideoCapture cap(0); // Initialize video capture from default camera (index 0)
    if(!cap.isOpened()){ // Check if camera is opened successfully
        cerr<<"Error: Unable to open camera"<<endl; // Display error message
        return -1; // Return error code
    }

    // Store detected corner locations for each image
    vector<vector<cv::Point2f>> cornerList;

    // Store 3D world positions of corners
    vector<cv::Vec3f> pointSet;

    // Store point sets for multiple images
    vector<vector<cv::Vec3f>> pointList;

    // Vector to store calibration images
    vector<cv::Mat> calibrationImages;

    while(true){

        // Capturing the frames
        cv::Mat frame;
        cap>>frame; // Read a frame from the camera

        // Convert frame to grayscale
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY); // Convert frame to grayscale

        // Find chessboard corners
        vector<cv::Point2f> corners;
        bool patternFound = cv::findChessboardCorners(gray, chessboardSize, corners); // Find chessboard corners in the grayscale image

        if(patternFound){

            // Refine corner locations
            cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001)); // Refine corner locations using sub-pixel accuracy

            // Draw corners on the image
            cv::drawChessboardCorners(frame, chessboardSize, cv::Mat(corners), patternFound); // Draw detected chessboard corners on the frame

            // Save corner locations and corresponding world points when 's' is pressed
            char key = cv::waitKey(0); // Wait for key press
            if(key=='s'){
                cornerList.push_back(corners); // Store detected corner locations

                // Generate 3D world points for the corners
                for(int i=0;i<chessboardSize.height;++i){
                    for(int j=0;j<chessboardSize.width;++j){
                        pointSet.push_back(cv::Vec3f(j,-i,0)); // Assuming Z=0 for all corners
                    }
                }

                pointList.push_back(pointSet); // Store 3D world points corresponding to corners
                calibrationImages.push_back(frame.clone()); // Store the calibration image

                // Clear the pointSet for the next calibration image
                pointSet.clear();

                // Print the number of corners and coordinates of the first corner
                cout<<"Number of corners:"<<corners.size()<<endl;
                if(!corners.empty()){
                    cout<<"Coordinates of the first corner: ("<<corners[0].x<<", "<<corners[0].y<<")"<<endl;
                }
            }
        }

        cv::imshow("frame", frame); // Display the frame

        // Break the loop if 'q' is pressed
        if(cv::waitKey(1) == 'q'){
            break; // Exit the loop
        }
    }

    // Release VideoCapture
    cap.release();

    // Check if enough calibration frames are selected
    if(cornerList.size() < 5){
        cerr<<"Error: Not enough calibration frames selected"<<endl; // Display error message
        return -1; // Return error code
    }

    // Calibrate the camera
    cv::Mat cameraMatrix = cv::Mat::eye(3,3, CV_64F); // Initialize camera matrix as identity matrix
    cv::Mat distCoeffs = cv::Mat::zeros(8,1, CV_64F); // Initialize distortion coefficients as zeros
    vector<cv::Mat> rvecs, tvecs; // Vectors to store rotation and translation vectors

    double rms = cv::calibrateCamera(pointList, cornerList, chessboardSize, cameraMatrix, distCoeffs, rvecs, tvecs); // Calibrate the camera

    // Print camera matrix and distortion coefficients
    cout<<"Camera matrix: "<< cameraMatrix<<endl;
    cout<<"Distortion coefficients: "<<distCoeffs<<endl;
    cout<<"Reprojection error: "<<rms<<endl;

    // Save intrinsic parameters to a file
    ofstream outFile("calibration_params.txt"); // Open file for writing
    if (outFile.is_open()) {
        outFile << "Camera matrix:" << endl;
        for (int i = 0; i < cameraMatrix.rows; ++i) {
            for (int j = 0; j < cameraMatrix.cols; ++j) {
                outFile << cameraMatrix.at<double>(i, j) << " "; // Write camera matrix to file
            }
            outFile << endl;
        }

        outFile << "Distortion coefficients:" << endl;
        // Save distortion coefficients for the fixed number of times
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < distCoeffs.cols; ++j) {
                outFile << distCoeffs.at<double>(i, j) << " "; // Write distortion coefficients to file
            }
            outFile << endl;
        }

        outFile << "Reprojection error:" << rms << endl;
        
        outFile.close(); // Close the file
        cout << "Intrinsic parameters saved to 'calibration_params.txt'." << endl;
    } else {
        cerr << "Error: Unable to write to file" << endl; // Display error message
    }

    // Show the calibration images with detected corners (optional)
    for (size_t i = 0; i < calibrationImages.size(); ++i) {
        cv::imshow("Calibration Image " + std::to_string(i+1), calibrationImages[i]); // Display each calibration image
        cv::waitKey(0); // Wait for key press
    }

    // Close all windows
    cv::destroyAllWindows();

    return 0; // Return success
}
