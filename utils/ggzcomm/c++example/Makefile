all: opcodes

source:
	moc -o connectx_client.moc.cpp connectx_client.h
	g++ -o client client.cpp connectx_client.cpp connectx_client.moc.cpp -I /usr/include/qt3/ -lqt-mt

opcodes:
	../ggzcommgen --language c++ --interface qt --mode debug --role client ../connectx.protocol
	../ggzcommgen --language c++ --interface qt --mode debug --role server ../connectx.protocol

xml:
	../ggzcommgen --xml --language c++ --interface qt --mode debug --role client ../connectx.protocol
	../ggzcommgen --xml --language c++ --interface qt --mode debug --role server ../connectx.protocol

clean:
	rm -f connectx_client.{cpp,h}
	rm -f connectx_server.{cpp,h}
	rm -f *.moc.cpp
	rm -f client server

