/*
    Saugat Malla
    Project 4
*/

/*
    Code for extension 3
*/

#include <iostream> // Header file for input and output operations
#include <opencv2/opencv.hpp> // OpenCV header file

using namespace std;

int main() {
    cv::VideoCapture cap(0); // Initialize video capture from camera
    if (!cap.isOpened()) { // If camera is not opened
        cerr << "Error: Unable to open camera" << endl; // Display error message
        return -1; // Return error code
    }

    // Load the pattern image
    cv::Mat pattern = cv::imread("checkerboard.jpg"); // Load pattern image
    if (pattern.empty()) { // If pattern image is not loaded
        cerr << "Error: Unable to load pattern image" << endl; // Display error message
        return -1; // Return error code
    }

    // Initialize ORB detector
    cv::Ptr<cv::ORB> orb = cv::ORB::create(); // Create ORB detector

    while (true) { // Infinite loop
        // Capture frame from camera
        cv::Mat frame;
        cap >> frame; // Capture frame from camera

        // Detect ORB features and compute descriptors
        vector<cv::KeyPoint> keypointsPattern, keypointsFrame; // Initialize keypoints vectors
        cv::Mat descriptorsPattern, descriptorsFrame; // Initialize descriptors matrices
        orb->detectAndCompute(pattern, cv::noArray(), keypointsPattern, descriptorsPattern); // Detect features and compute descriptors for pattern image
        orb->detectAndCompute(frame, cv::noArray(), keypointsFrame, descriptorsFrame); // Detect features and compute descriptors for frame

        // Match descriptors
        cv::BFMatcher matcher(cv::NORM_HAMMING); // Initialize BFMatcher
        vector<cv::DMatch> matches; // Initialize matches vector
        matcher.match(descriptorsPattern, descriptorsFrame, matches); // Match descriptors between pattern and frame

        // Draw matches on frame
        cv::Mat imgMatches; // Initialize image for matches
        cv::drawMatches(pattern, keypointsPattern, frame, keypointsFrame, matches, imgMatches); // Draw matches on frame

        // Show the frame with matches
        cv::imshow("Matches", imgMatches); // Display frame with matches

        // Break the loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') { // If 'q' key is pressed
            break; // Exit the loop
        }
    }

    cap.release(); // Release the camera
    cv::destroyAllWindows(); // Close all OpenCV windows

    return 0; // Return success
}
