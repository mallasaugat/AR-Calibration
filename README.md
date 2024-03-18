# AR-Calibration

## Overview:
- Calibrate a camera and then use the calibration to generate virtual objects in a scene.
- The program detects a target and then place a virtual object in the scene relative to the target that moves and orients itself correctly given motion of the camera or target. 

## Implementation:
- Detect and Extract Target Corners:
    - System that detects a target and extracts target corners.    
- Select Calibration Images:
    - Image used for calibration.
    - Save the corner locations and the correcponding 3D world points.
    - If user key presses 's', then we store the vector of corners found by the last successful target detection into a corner_list.
    - Chessboard corners highlighted.
- Calibrate the Camera:
    - Calibration is continuously updated each time the user adds a new calibration beyond some minium number.
    - Get the current per pixel error after each calibration.
    - Error estimation.
    Calibration matirix along with corresponding re-projection error.
- Calculate current position of the camera:
    - Read the camera calibration parameters from a file and then start a video loop.
    - For each frame, it tries to detect a target.
    - If found, it grabs the locations of the corners, and then uses [solvePNP](https://docs.opencv.org/3.4/d9/d0c/group__calib3d.html#ga549c2075fac14829ff4a58bc931c033d) to get the board's pose (rotation and translation).
- Project outside corners or 3D Axes:
    - Using [projectPoints](https://docs.opencv.org/3.4/d9/d0c/group__calib3d.html#ga1019495a2c8d1743ed5cc23fa0daff8c) function to project the 3D points corresponding to at least four corners of the target onto the image plane in real time as the target or camera moves around.
    - 3D axes on the target attached to the origin which will help build virtual object.
- Creating virtual object
    - Construct a virtual object in 3D world space made out of lines that floats above the board.
    - Project the virtual object to the image and draw the lines in the image whie the object stays in the correct orientation as the camera moves around.
- Detect Robust Features 
    - Harris corners and SURF features.
    - Program that shows where the features are in the image in a video stream.
    - Generate a pattern and display where the features show up in the pattern.
    - Different thesholds and setting are used to for features.