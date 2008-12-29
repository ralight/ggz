protocolhandler:
	ggzcommgen --language c --interface easysock --role $(ROLE) $(PROTOCOL)
	mv `basename $(PROTOCOL)_$(ROLE).c` $(SOURCEFILE).c
	mv `basename $(PROTOCOL)_$(ROLE).h` $(SOURCEFILE).h
	-indent -kr -i8 -nsai -nsaf -cli8 -bl -bli0 $(SOURCEFILE).c $(SOURCEFILE).h
	base=`basename $(PROTOCOL)_$(ROLE).h` && \
	sed -i "s/$$base/$(SOURCEFILE).h/" $(SOURCEFILE).c

