# Copyright (C) 2021 Dalei Liu

LIBRE_MK := re/Makefile
LIBREM_MK := rem/Makefile

all: $(LIBRE_MK) $(LIBREM_MK)
	make -C src
	make -C modules modbins
	make -C apps/replica

$(LIBRE_MK) $(LIBREM_MK):
	git submodule update --init

deb: $(LIBRE_MK) $(LIBREM_MK)
	dpkg-buildpackage -b

deb-clean:
	cd debian; while read fn; do echo $$fn; rm -rf $$fn; done < .gitignore
