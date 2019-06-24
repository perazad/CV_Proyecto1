
all:

	g++ -ggdb `pkg-config opencv --cflags` proyecto01.cpp -o `basename proyecto01.cpp .cpp` `pkg-config opencv --libs` -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -ldl -lm
	
clean:

	rm proyecto01.o
