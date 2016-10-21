CXXFLAGS=`wx-config --cxxflags` -std=c++11 -ggdb3 -O0 -DDEBUG_PROPERTY
LDFLAGS=`wx-config --libs`

main: main.cpp property_mgr.cpp

clean:
	rm main || true
