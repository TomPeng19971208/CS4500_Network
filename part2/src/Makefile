build:
	g++ -w -std=c++11 server.cpp -o server
	g++ -w -std=c++11 client.cpp -o client
run:
	sudo ifconfig lo0 alias 127.0.0.2
	sudo ifconfig lo0 alias 127.0.0.3
	sudo ifconfig lo0 alias 127.0.0.4
	./server -ip 127.0.0.1 &
	./client -ip 127.0.0.2 &
	./client -ip 127.0.0.3 &
	./client -ip 127.0.0.4
