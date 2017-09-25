

LDFLAGS+=-lglut -lGL -lm -lGLU

sim_blazers: sim.c makeColor.cpp ./FakeOcto/OctoWS2811.cpp Blazers.cpp
	g++ -D SIM_NAME=blazers -I./FakeOcto/ $^ -o $@ $(LDFLAGS)


sim_neutrinos: sim.c makeColor.cpp ./FakeOcto/OctoWS2811.cpp Neutrinos.cpp
	g++ -D SIM_NAME=neutrinos -I./FakeOcto/ $^ -o $@ $(LDFLAGS)


