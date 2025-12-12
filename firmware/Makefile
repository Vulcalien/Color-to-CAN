.PHONY: all clean distclean genromfs program reset qconfig

ROMFS := config/include/romfs.h
RCS   := etc/init.d/rcS

all: submodules/nuttx/.config genromfs
	cd submodules/nuttx && $(MAKE) all && arm-none-eabi-size nuttx

clean: submodules/apps/apps
	$(MAKE) -C submodules/nuttx clean
	-rm $(ROMFS) $(RCS)

distclean: submodules/apps/apps
	$(MAKE) -C submodules/nuttx distclean
	-rm submodules/apps/apps $(ROMFS) $(RCS)

genromfs:
	@if [ "$(ID)" = "" ]; then\
		echo Error: please use 'make ID=num';\
		exit 1;\
	fi
	mkdir -p $(dir $(RCS))
	sed "s/{{SENSOR_ID}}/$(ID)/" rcS.template > $(RCS)
	genromfs -f romfs.img -d etc -V 'ROMFS /etc'
	xxd -i romfs.img $(ROMFS)
	-rm romfs.img

program: all
	openocd \
        -f interface/stlink.cfg \
        -f target/stm32l4x.cfg  \
        -c "adapter speed 50"   \
        -c "program submodules/nuttx/nuttx verify reset exit"

reset:
	openocd \
        -f interface/stlink.cfg \
        -f target/stm32l4x.cfg  \
        -c "adapter speed 50"   \
        -c init -c reset -c exit

qconfig: submodules/nuttx/.config
	$(MAKE) -C submodules/nuttx qconfig
	cp -v submodules/nuttx/.config config/tof-l431-nsh/defconfig

submodules/apps/apps:
	ln -sTf ../../apps submodules/apps/apps

submodules/nuttx/.config: submodules/apps/apps
	cd submodules/nuttx/tools && ./configure.sh ../../config/tof-l431-nsh
