all:
	@echo either use "make full_dist", "make dist_wii" or "make dist_vwii"

REV=$(shell grep define svnrev/svnrev.c | gawk '{print $$3}')

full_dist: dist_wii dist_vwii pack_forwarder

dist_wii: compile_installer compile_stboot
	@tar cfj dist/HomebrewFilter-rev$(REV).tar.bz2 dist/HomebrewFilter/
	@tar cfj dist/HomebrewFilter-Standalone-rev$(REV).tar.bz2 dist/HomebrewFilter.Standalone/

dist_vwii: compile_installer_vwii compile_stboot_vwii
	@tar cfj dist/HomebrewFilter-vWii-rev$(REV).tar.bz2 dist/HomebrewFilter.vWii/
	@tar cfj dist/HomebrewFilter-vWii-Standalone-rev$(REV).tar.bz2 dist/HomebrewFilter.vWii.Standalone/

compile_forwarder:
	@make -C forwarder
	@cp forwarder/forwarder.dol forwarder/wad/00000001.app
	@cp forwarder/forwarder.dol forwarder/wad.vwii/00000001.app

pack_forwarder: compile_forwarder
	@wine tools/WadMii.exe -input "Z:$(PWD)\forwarder\wad" -output "Z:$(PWD)\dist\HBF0.wad"
	@wine tools/WadMii.exe -input "Z:$(PWD)\forwarder\wad" -output "Z:$(PWD)\dist\HBF0.vWii.wad"

distclean: clean
	@rm -f dist/HomebrewFilter*/boot.dol
	@rm -f dist/HomebrewFilter*.tar.bz2
	@rm -f dist/HBF0*.wad

clean:
	@make -C main clean
	@make -C boot clean
	@make -C installer clean
	@make -C nand-loader clean
	@make -C forwarder clean
	@make -C libruntimeiospatch clean

clean_vwii:
	@make -C main clean
	@make -C boot clean
	@make -C installer clean

nand_loader:
	@make -C nand-loader

libruntimeiospatch:
	@make -C libruntimeiospatch

compile_stboot: clean
	@echo "============================"
	@echo "== HBF Standalone for Wii =="
	@echo "============================"
	@rm -f dist/HomebrewFilter.Standalone/boot.dol
	@make -C main clean
	@XFLAGS="-DSTBOOT" make -C main
	@cp main/hbf.dol boot/source/hbf.dol
	@make -C boot
	@cp boot/hbf_boot.dol dist/HomebrewFilter.Standalone/boot.dol

compile_stboot_vwii: clean_vwii
	@echo "============================="
	@echo "== HBF Standalone for vWii =="
	@echo "============================="
	@rm -f dist/HomebrewFilter.vWii.Standalone/boot.dol
	@make -C main clean
	@XFLAGS="-DSTBOOTVWII -DVWII" make -C main
	@cp main/hbf.dol boot/source/hbf.dol
	@make -C boot
	@cp boot/hbf_boot.dol dist/HomebrewFilter.vWii.Standalone/boot.dol

compile_hbf: clean
	@echo "==========================="
	@echo "== HBF Installer for Wii =="
	@echo "==========================="
	@rm -f dist/HomebrewFilter/boot.dol
	@make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot: compile_hbf
	@make -C boot
	@cp boot/hbf_boot.dol installer/wad/00000001.app

compile_installer: compile_boot
	@wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol dist/HomebrewFilter/boot.dol

compile_hbf_vwii: clean_vwii
	@echo "============================"
	@echo "== HBF Installer for vWii =="
	@echo "============================"
	@rm -f dist/HomebrewFilter.vWii/boot.dol
	@XFLAGS="-DVWII" make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot_vwii: compile_hbf_vwii
	@make -C boot
	@cp boot/hbf_boot.dol installer/wad.vwii/00000002.app

compile_installer_vwii: compile_boot_vwii
	@wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad.vwii" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol dist/HomebrewFilter.vWii/boot.dol
