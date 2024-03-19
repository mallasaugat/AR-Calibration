/*
    Saugat Malla
    Project 4
*/

/*
    Code for Task 7
*/

#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp> // Include this for SURF

using namespace cv;
using namespace cv::xfeatures2d; // Add this namespace for SURF
using namespace std;

int main() {
    // Open webcam
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cerr << "Error: Couldn't access the webcam." << endl;
        return -1;
    }

    Mat frame, frame_gray;
    vector<KeyPoint> keypoints;
    Mat descriptors;
    Ptr<Feature2D> surf = SURF::create(); // Initialize SURF detector

    namedWindow("SURF Feature Detection", WINDOW_NORMAL);

    while (true) {
        cap >> frame; // Capture frame from webcam

        // Convert frame to grayscale
        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        // Detect SURF keypoints and compute descriptors
        surf->detectAndCompute(frame_gray, Mat(), keypoints, descriptors);

        // Draw keypoints on the original image
        Mat surf_keypoints;
        drawKeypoints(frame, keypoints, surf_keypoints);

        // Display the resulting frame
        imshow("SURF Feature Detection", surf_keypoints);

        // Exit when 'q' is pressed
        if (waitKey(1) == 'q')
            break;
    }

    // Release the camera
    cap.release();
    destroyAllWindows();

    return 0;
}
