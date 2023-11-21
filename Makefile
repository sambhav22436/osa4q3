CC = gcc
CFLAGS = -pthread

all: bridge_simulation

bridge_simulation: bridge_simulation.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f bridge_simulation
