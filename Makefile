all:
	@echo either use "make dist" or "make dist_vwii"

REV=$(shell grep define svnrev/svnrev.c | gawk '{print $$3}')

dist: compile_installer
	@tar cfj HomebrewFilter-rev$(REV).tar.bz2 HomebrewFilter/
	@tar cfj HomebrewFilter-Standalone-rev$(REV).tar.bz2 HomebrewFilter.Standalone/

dist_vwii: compile_installer_vwii
	@tar cfj HomebrewFilter-vWii-rev$(REV).tar.bz2 HomebrewFilter.vWii/
	@tar cfj HomebrewFilter-vWii-Standalone-rev$(REV).tar.bz2 HomebrewFilter.vWii.Standalone/

clean:
	@make -C main clean
	@make -C boot clean
	@make -C installer clean
	@make -C nand-loader clean
	@make -C libruntimeiospatch clean

clean_vwii:
	@make -C main -f Makefile.vWii clean
	@make -C boot clean
	@make -C installer clean

nand_loader:
	@make -C nand-loader

libruntimeiospatch:
	@make -C libruntimeiospatch

compile_hbf: clean
	@echo "==========================="
	@echo "======= HBF for Wii ======="
	@echo "==========================="
	@make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot: compile_hbf
	@make -C boot
	@cp boot/hbf_boot.dol installer/wad/00000001.app
	@cp boot/hbf_boot.dol HomebrewFilter.Standalone/boot.dol

compile_installer: compile_boot
	@tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol HomebrewFilter/boot.dol

compile_hbf_vwii: clean_vwii
	@echo "============================"
	@echo "======= HBF for vWii ======="
	@echo "============================"
	@CFLAGS="$(CFLAGS) -DVWII" make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot_vwii: compile_hbf_vwii
	@make -C boot
	@cp boot/hbf_boot.dol HomebrewFilter.vWii.Standalone/boot.dol
	@cp boot/hbf_boot.dol installer/wad.vwii/00000002.app

compile_installer_vwii: compile_boot_vwii
	@tools/WadMii.exe -input "Z:$(PWD)\installer\wad.vwii" -output "Z:$(PWD)\installer\data\install.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad.vwii" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol HomebrewFilter.vWii/boot.dol
