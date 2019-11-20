all: clean servidor cliente

servidor:
	g++ -o servidor ./servidor.cpp -lpthread

cliente:
	g++ -o cliente ./cliente.cpp

clean:
	rm -rf *.o cliente
	rm -rf *.o servidor