TARGET = Movie_bgr
opencv_path = /usr/local
CXX = g++
CFLAGS += -Wl,--rpath=/usr/local/lib \
		  -I$(opencv_path)/include \
		  -I$(opencv_path)/include/opencv \
		  -I$(opencv_path)/include/opencv2 \
		  -L/usr/lib \
		  -L$(opencv_path)/lib
CFLAGS +=  -lopencv_core -lopencv_highgui  -lopencv_imgproc -lpthread -lrt

all:
	@$(CXX)  $(TARGET).cpp -o $(TARGET) $(CFLAGS)

clean:
	rm -rf $(TARGET) *.o