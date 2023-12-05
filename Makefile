CXX = g++
CXXFLAGS = -W -g

mytest: mytest.cpp dealer.cpp 
	$(CXX) $(CXXFLAGS) mytest.cpp dealer.cpp -o mytest

run:
	./mytest