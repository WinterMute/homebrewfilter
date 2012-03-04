#! /bin/bash
#

# XXX move into mkrelease rule

#erstelle meta.xml hbf
cat <<EOF > "$PWD"/meta.xml.boot
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<app version="1">
  <name>Homebrew Filter</name>
  <coder>Christopher Roy Bratusek (Nano)<br>
         hamachi-mp</coder>
  <version>r$rev_new</version>
  <no_ios_reload/>
  <short_description>Homebrew Filter / Sorter</short_description>
  <long_description>Dieses HB-App dient zur Sortierung und Kategorisierung Ihrer Homebrews</long_description>
</app>
EOF

#erstelle meta.xml hbf installer
cat <<EOF > "$PWD"/meta.xml.installer
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<app version="1">
  <name>Homebrew Filter Installer</name>
  <coder>Christopher Roy Bratusek (Nano)<br>
         hamachi-mp</coder>
  <version>r$rev_new</version>
  <no_ios_reload/>
  <short_description>Installer</short_description> 
  <long_description>Installiert den Hombrew Filter</long_description> 
</app>
EOF
