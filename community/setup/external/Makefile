all:
	@echo "make 'fetch' to get the tarballs"

fetch:
	@for i in `grep -v ^# tarballs.txt | tr \\\\n " "`; do echo $$i; done | while read url; do \
		read md5; \
		read file; \
		xmd5=`md5sum $$file 2>/dev/null | cut -d " " -f 1`; \
		if test "x$$xmd5" = "x$$md5"; then \
			echo "skip $$file"; \
		else \
			if test -f $$file; then \
				echo "continue $$file"; \
			else \
				echo "download $$file"; \
			fi; \
			wget --quiet -c $$url; \
		fi; \
	done

