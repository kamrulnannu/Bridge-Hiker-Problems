src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)

LDFLAGS = -lyaml

bridge_hiker: $(obj)
	g++ -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) bridge_hiker
