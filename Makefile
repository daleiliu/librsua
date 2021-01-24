# Copyright (C) 2021 Dalei Liu

LIBRE_MK := re/Makefile
LIBREM_MK := rem/Makefile

all: $(LIBRE_MK) $(LIBREM_MK)
	make -C src
	make -C modules modbins
	make -C apps/replica

$(LIBRE_MK) $(LIBREM_MK):
	git submodule update --init

.PHONY: deb deb-clean
deb: $(LIBRE_MK) $(LIBREM_MK) debian/copyright
	dpkg-buildpackage -b

debian/copyright: docs/copyright docs/license
	CSTR="$$(while read line; \
		do printf " %s\134n" "$$line"; done \
		< docs/copyright)"; \
	  LSTR="$$(while read line; \
		do if [ -z "$$line" ]; then printf " .\134n"; \
		else printf " %s\134n" "$$line"; fi; done < docs/license)"; \
	  sed -e "s/COPYRIGHT/$${CSTR}/" -e "s#LICENSE#$${LSTR}#" \
		debian/copyright.tmpl > debian/copyright

deb-clean:
	cd debian; while read fn; do echo $$fn; rm -rf $$fn; done < .gitignore
