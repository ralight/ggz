all: opcodes

opcodes:
	../ggzcommgen --language python --interface socket --mode debug --role client ../connectx.protocol
	../ggzcommgen --language python --interface socket --mode debug --role server ../connectx.protocol

xml:
	../ggzcommgen --xml --language python --interface socket --mode debug --role client ../connectx.protocol
	../ggzcommgen --xml --language python --interface socket --mode debug --role server ../connectx.protocol

clean:
	rm -f connectx_client.py
	rm -f connectx_server.py
	rm -f *.pyc

