//rev42:
- changed default options:
  * disable checking for new version automatically
  * enable sd-card debug logging
- added option for choosing whether bootmii boot2 is
  installed (adds "Load BootMii (Boot2)" in Exit-Prompt
- black-listed v65280 of IOS254 (if that version is
  installed there's no "Load BootMii (IOS)" anymore in Exit-Prompt
- ensure 'Launch Priiloader' is only shown if Priiloader
  is really installed (in some cases it could appear even
  if Priiloader hasn't been installed)
- fixed a bug that caused black-screen then returning into
  HBF from app loaded with <ahb_access/>
- build separate vWii-version using FIX94s NAND-Loader (still using
  modified Waninkoko NAND-Loader on real Wii)
- build using libruntimeiospatch 1.3
- vWii version does disable functions not applicable von vWii
- installer now falls back to IOS236 if there's no HW_AHBPROT access
- SDCardGecko (Debugfile) is now stored on general settings device
  (no more hardcoded to SD Card)

//rev41:
- fixed auto-connecting Internet via WiFi
- fixed video initialization problem at PAL 50Hz
- fixed categorizing apps from SD Gecko
- removed remaining references to HBC
- basic support for WifiGecko
  * most messages are shown via WifiGecko. Those that are
    sent prior to network initialization will be missing
  * option to en/disable WifiGecko (settings->network)
  * no option yet to change IP and Port, currently sending
    to 192.168.1.4:4405 (you change your PCs IP from Router
    configuration interface)
- support for SDCardGecko (not SD Gecko!)
  * saves debug messages to SD:/config/HBF/debug.txt
  * endless logging, different logs are split with an
    "HBF Log (YYYY-MM-DD / HH:MM:SS)" message. HBF never
    deletes the logfile on it's own.
Note: ** Gecko priority:
	1. SD Card Gecko
	2. Wifi Gecko
	3. USB Gecko
	if SD Card Gecko is enabled, there's no messages
	send via Wifi or USB and so on. By default HBF
	tries to connect to USB Gecko.

//rev40:
- new icon (JoostinOnline)
- catalan language downloadble (Adrin Lavis)
- added support for SD-Gecko (both Slot A and B, no hot-plugging)
- fixed a regression from rev39. <no_ios_reload> is now
  treated correctly (makes WiiMC properly detect HDD)
- fixed a regression from rev37. don't loop three times
  when selected storage device is DVD
- added <force_no_ios_reload/> support, for testing purposes
- fixed clean rule in main/Makefile
- add 'Restart HBF' option to exit prompt
- renamed 'external loaders' prompt to 'external applications'
- add 'Start SE GUI' option to external applications prompt
  must be :/apps/SettingsEditorGUI/boot.dol on any device
  (all sd, usb, dvd, sd gecko a and sd gecko b supported)
  Now SettingsEditorGUI is now longer shown in the Homebrew-list
- remove back button from exit prompt (press B/HOME to go back)
- fixed a bug in online-updates (if online-update fails,
  simply boot 'The HomebrewFilter Installer', the updated
  boot.dol is already saved on your SD-Card (or USB-HDD))
  -- future online-updates will work normally again.
- For theme designers: the following new icons have been added:
	* gca_active (device menu)
	* gca_inactive (device menu)
	* gcb_active (device menu)
	* gcb_inactive (device menu)
	* choice_large (not new, but wider now)

//rev39:
- BUGFIX: before saving the update boot.dol, ensure directory
          apps/HomebrewFilter exists, if not create it
- overall improvements to the NAND-loader in THBF channel (obcd)
- BUGFIX: properly check whether we're in neek2o environment (obcd)
- improved neek2o booter (obcd)
- support for SD-Gecko and neek2o in HBF booter (obcd)
- improved meta.xml arguements for IOS-reload/HW_AHBPROT
  - <ahb_access/>: preserve HW_AHBPROT and reload the selected IOS
  - <no_ios_reload/>: alias for <ahb_access/> (compat with HBC)
  - <force_ios_reload/> or nothing: just reload the selected IOS
  - if we are in neek2o we do nothing
  - for wiiload there are now two options to choose which IOS to
    use and whether to reload it
- BUGFIX: fix compilation with libOGC 1.8.11 and newer
- BUGFIX: installer upon exit checks whether THBF channel is installed,
          if so, it will start the HomebrewFilter, else it enters SysMenu
- Add 'Reinstall the HomebrewFilter' option in the installer. If the
  installation fails for you, when you update from an older version use
  this option. (the failure is caused by IOS58 not beeing able to
  downgrade an existing channel - old THBF channel had version 512,
  new THBF channel has same version than the application.)

//rev38:
- changes to the app-info dialog:
	* DVD icon is now shown if necessary
	* erase button is hidden for apps from DVD
	* edit button no longer overlaps SD/USB/DVD icon (if any)
- BUGFIX: childlock is now saved when HBF exits
- BUGFIX: call ExitApp() before exiting HBF
- BUGFIX: installer no longer crashes on IOS58 due to early
          controller initialization (only happened under
	  certain circumstances)
- BUGFIX: use waninkoko's NAND loader in THBF channel,
          rather than comex'.
	  * Fixes the issue that HBF does not properly
	  respond when power or reset button on the console
	  have been pressed.
	  * the strange bevhaviour that occured when the
	  USB HDD was woken up while loading HBF is gone
- BUGFIX: compiled with libpng 1.4 to fix a problem in PNG handling
- BUGFIX: return to HBF feature now works again
- installer now supports Classic-Controller
- installer now supports GameCube-Controller
- installer loads HomebrewFilter rather than returning to Systemmenu
- installer no longer invokes update-counter on hamachi-mp's website
- add support for <force_ios_reload/> flag in meta.xml (use this, if
  an application (most likely WiiMC) does not recognize your HDD)
- adopt libwiigui changes from ZERO
- BUGFIX: loading apps through wiiload works again
- support passing arguements using wiiload [obcd]
- BUGFIX: when changing network settings, the network thread is
          halted or started accordingly [obcd]
- BUGFIX: only show 'launch priiloader' in external loaders prompt,
          if priiloader is really installed
- HBF now supports wiiload over USB-Gecko [obcd]
- changed loading address from 0x81230000 to 0x81330000
- removed all sound related code
- support for neek2o [obcd]
- support for switching to neek2o/real NAND
  * when in real NAND and neek2o files present on either SD
    or the first partiton of USB HDD 'external loaders' menu
    offers a 'launch neek2o' button to enter neek2o emu NAND
  * when in neek2o emu NAND 'external loaders' menu offers an
    'enter real NAND' button to switch back to real NAND
- only check once on startup for a new version [obcd]
- updated PNGu