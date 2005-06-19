all:
	@echo "make messages install"

install:
	@for i in *.po; do \
		b=$$(echo $$i | cut -d "." -f 1); \
		echo Install $$i; \
		mkdir -p $$b/LC_MESSAGES; \
		msgfmt -o $$b/LC_MESSAGES/ggzcommunity.mo $$i; \
	done

messages:
	@echo Catalog update: templates
	@x="-name *.rightbar -o -name *.leftbar"; \
	x="$$x -o -name *.inc -o -name *.php"; \
	l=$$(find ../.. $$x); \
	./extract.pl $$l > ggzcommunity.pot
	@echo Catalog update: source
	@./extract-php.pl ../*.php >> ggzcommunity.pot
	#@xgettext --from-code utf-8 --omit-header -LPHP -k__ ../*.php -o tmp
	#cat tmp >> ggzcommunity.pot
	rm -f tmp
	@for i in *.po; do \
		echo Merge $$i; \
		msgmerge $$i ggzcommunity.pot > tmp && mv tmp $$i; \
	done

