all: servidor cliente

servidor:
	g++ -o servidor ./servidor.cpp -lrt -lpthread

cliente:
	g++ -o cliente ./cliente.cpp -lrt -lpthread

clean:
	rm -rf *.o cliente
	rm -rf *.o servidor