crawler:crawler.cpp
	g++  -o $@ $^ -lcurl -lboost_regex

.PHONY:clean
clean:
	rm -rf crawler
