CXXFLAGS=`wx-config --cxxflags` -std=c++11 -ggdb3 -O0
LDFLAGS=`wx-config --libs`

main: main.cpp property.cpp property_mgr.cpp