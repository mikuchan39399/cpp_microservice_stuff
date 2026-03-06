log_test:log_test.cc ../source/exSpdlog.cc
	g++ -std=c++20 -o $@ $^ -lspdlog -lpthread -lfmt -lgflags -I../source

.PHONY:clean
clean:
	rm -f log_test
