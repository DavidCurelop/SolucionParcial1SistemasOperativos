CXX = g++
CXXFLAGS = -O2 -std=c++17 -Wall -Wextra

OBJS = main_parcial.o analytics.o utils.o generador.o persona.o

parcial: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

main_parcial.o: main_parcial.cpp persona.h generador.h analytics.h utils.h persona_c.h
	$(CXX) $(CXXFLAGS) -c main_parcial.cpp

analytics.o: analytics.cpp analytics.h persona.h persona_c.h utils.h
	$(CXX) $(CXXFLAGS) -c analytics.cpp

utils.o: utils.cpp utils.h persona.h persona_c.h
	$(CXX) $(CXXFLAGS) -c utils.cpp

generador.o: generador.cpp generador.h persona.h
	$(CXX) $(CXXFLAGS) -c generador.cpp

persona.o: persona.cpp persona.h
	$(CXX) $(CXXFLAGS) -c persona.cpp

clean:
	rm -f *.o parcial
