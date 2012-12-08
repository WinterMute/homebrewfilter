all: compile_installer

clean:
	@make -C main clean
	@make -C boot clean
	@make -C installer clean
	@make -C nand-loader clean

compile_hbf:
	@make -C main
	@cp main/hbf.dol boot/source/hbf.dol

compile_boot: compile_hbf
	@make -C boot
	@cp boot/hbf_boot.dol installer/wad/00000001.app

compile_nandloader: compile_boot
	@make -C nand-loader
	#@cp nand-loader/nand-loader.bin installer/wad/00000002.app

compile_installer: compile_nandloader
	@tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad" || wine tools/WadMii.exe -input "Z:$(PWD)\installer\wad" -output "Z:$(PWD)\installer\data\install.wad"
	@make -C installer
	@cp installer/installer.dol HomebrewFilter/boot.dol
