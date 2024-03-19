CXX = g++
CXXFLAGS = -std=c++11
LIBS = -L/opt/homebrew/lib -lopencv_imgcodecs -lopencv_imgproc -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_videostab -lopencv_objdetect -lopencv_dnn -lopencv_calib3d -lopencv_features2d -lopencv_xfeatures2d
INCLUDES = -I/opt/homebrew/include/opencv4


cameraCalibration: cameraCalibration.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

projection: projection.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

surf: surf.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

projectionGrid: projectionGrid.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

projectionCube: projectionCube.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

featureORB: featureORB.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)


.PHONY: clean

clean:
	rm -f cameraCalibration projection projectionGrid projectionCube featureORB
