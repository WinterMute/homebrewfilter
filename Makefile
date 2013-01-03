all:
	@echo either use "make dist" or "make dist_vwii"

REV=$(shell grep define svnrev/svnrev.c | gawk '{print $$3}')

full_dist: dist dist_vwii pack_forwarder

dist: compile_installer compile_stboot
	@tar cfj HomebrewFilter-rev$(REV).tar.bz2 HomebrewFilter/
	@tar cfj HomebrewFilter-Standalone-rev$(REV).tar.bz2 HomebrewFilter.Standalone/

dist_vwii: compile_installer_vwii compile_stboot_vwii
	@tar cfj HomebrewFilter-vWii-rev$(REV).tar.bz2 HomebrewFilter.vWii/
	@tar cfj HomebrewFilter-vWii-Standalone-rev$(REV).tar.bz2 HomebrewFilter.vWii.Standalone/

pack_forwarder:
	@tools/WadMii.exe -input "Z:$(PWD)\forwarder\wad" -output "Z:$(PWD)\HBF0.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\forwarder\wad" -output "Z:$(PWD)\HBF0.wad"
	@tools/WadMii.exe -input "Z:$(PWD)\forwarder\wad.vwii" -output "Z:$(PWD)\HBF0.vWii.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\forwarder\wad" -output "Z:$(PWD)\HBF0.vWii.wad"

clean:
	@make -C main clean
	@make -C boot clean
	@make -C installer clean
	@make -C nand-loader clean
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
	@rm -f HomebrewFilter.Standalone/boot.dol
	@make -C main clean
	@XFLAGS="-DSTBOOT" make -C main
	@cp main/hbf.dol boot/source/hbf.dol
	@make -C boot
	@cp boot/hbf_boot.dol HomebrewFilter.Standalone/boot.dol

compile_stboot_vwii: clean_vwii
	@echo "============================="
	@echo "== HBF Standalone for vWii =="
	@echo "============================="
	@rm -f HomebrewFilter.vWii.Standalone/boot.dol
	@make -C main clean
	@XFLAGS="-DSTBOOTVWII -DVWII" make -C main
	@cp main/hbf.dol boot/source/hbf.dol
	@make -C boot
	@cp boot/hbf_boot.dol HomebrewFilter.vWii.Standalone/boot.dol

compile_hbf: clean
	@echo "==========================="
	@echo "== HBF Installer for Wii =="
	@echo "==========================="
	@rm -f HomebrewFilter/boot.dol
	@make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot: compile_hbf
	@make -C boot
	@cp boot/hbf_boot.dol installer/wad/00000001.app

compile_installer: compile_boot
	@tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol HomebrewFilter/boot.dol

compile_hbf_vwii: clean_vwii
	@echo "============================"
	@echo "== HBF Installer for vWii =="
	@echo "============================"
	@rm -f HomebrewFilter.vWii/boot.dol
	@XFLAGS="-DVWII" make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot_vwii: compile_hbf_vwii
	@make -C boot
	@cp boot/hbf_boot.dol installer/wad.vwii/00000002.app

compile_installer_vwii: compile_boot_vwii
	@tools/WadMii.exe -input "Z:$(PWD)\installer\wad.vwii" -output "Z:$(PWD)\installer\data\install.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad.vwii" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol HomebrewFilter.vWii/boot.dol
