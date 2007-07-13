protocolhandler:
	$(top_srcdir)/../utils/ggzcomm/ggzcommgen \
		--language c --interface easysock --role server \
		$(top_srcdir)/../utils/ggzcomm/$(PROTOCOL)
	mv $(PROTOCOL)_server.c $(SOURCEFILE).c
	mv $(PROTOCOL)_server.h $(SOURCEFILE).h
	indent -kr -i8 -nsai -nsaf -cli8 -bl -bli0 $(SOURCEFILE).c $(SOURCEFILE).h
	sed -i 's/$(PROTOCOL)_server.h/$(SOURCEFILE).h/' $(SOURCEFILE).c

