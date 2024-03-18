CXX = g++
CXXFLAGS = -std=c++11
# LIBS = -L/opt/homebrew/lib -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_videostab -lopencv_objdetect -lopencv_dnn
# LIBS = -L/opt/homebrew/lib -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_videostab -lopencv_objdetect -lopencv_dnn -lopencv_calib3d
LIBS = -L/opt/homebrew/lib -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_videostab -lopencv_objdetect -lopencv_dnn -lopencv_calib3d -lopencv_features2d


INCLUDES = -I/opt/homebrew/include/opencv4


cameraCalibration: cameraCalibration.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

projection: projection.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -f cameraCalibration
