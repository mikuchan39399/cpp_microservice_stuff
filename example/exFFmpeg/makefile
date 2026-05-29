all: hls_test
hls_test: hls_test.cc ../../source/exFFmpeg.cc ../../source/exSpdlog.cc ../../source/util.cc
	g++ -g -std=c++20 -o $@ $^ -lavcodec -lavformat -lavutil -lfmt -lspdlog -ljsoncpp
.PHONY:clean
clean:
	rm -f hls_test dest*