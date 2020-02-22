build:
	g++ -pthread -w -std=c++11 server.cpp -o server
	g++ -pthread -w -std=c++11 client.cpp -o client
run:
	./server -ip 127.0.0.1 &
	./client -ip 127.0.0.2 &
	./client -ip 127.0.0.3 &
	./client -ip 127.0.0.4
