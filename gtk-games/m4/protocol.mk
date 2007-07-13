protocolhandler:
	$(top_srcdir)/../utils/ggzcomm/ggzcommgen \
		--language c --interface easysock --role client \
		$(top_srcdir)/../utils/ggzcomm/$(PROTOCOL)
	mv $(PROTOCOL)_client.c $(SOURCEFILE).c
	mv $(PROTOCOL)_client.h $(SOURCEFILE).h
	indent -kr -i8 -nsai -nsaf -cli8 -bl -bli0 $(SOURCEFILE).c $(SOURCEFILE).h
	sed -i 's/$(PROTOCOL)_client.h/$(SOURCEFILE).h/' $(SOURCEFILE).c

