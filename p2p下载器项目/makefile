all:main
main:main.cpp
	g++ -std=c++14 -DCPPHTTPLIB_OPENSSL_SUPPORT $^ -o $@ -lboost_filesystem -lboost_system -lpthread -lboost_thread -lssl -lcrypto
