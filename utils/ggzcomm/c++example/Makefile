all: gen-stamp source

gen-stamp:
	$(MAKE) xml
	touch $@

source:
	moc -o connectx_client.moc.cpp connectx_client.h
	moc -o connectx_server.moc.cpp connectx_server.h
	moc -o server.moc.cpp server.h
	moc -o client.moc.cpp client.h
	g++ -Wall -o client client.cpp connectx_client.cpp connectx_client.moc.cpp client.moc.cpp -I /usr/include/qt3/ -lqt-mt
	g++ -Wall -o server server.cpp connectx_server.cpp connectx_server.moc.cpp server.moc.cpp -I /usr/include/qt3/ -lqt-mt

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
	rm -f gen-stamp

