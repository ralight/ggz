all:
	@echo "make 'fetch' to get the tarballs"

fetch:
	@for url in `grep -v ^# tarballs.txt`; do \
		wget -c $$url; \
	done

