json_test:json_test.cc ../../source/util.cc ../../source/exSpdlog.cc
	g++ -std=c++20 -o $@ $^ -lspdlog -lpthread -lfmt -ljsoncpp -I../../source

.PHONY:clean
clean:
	rm -f json_test
