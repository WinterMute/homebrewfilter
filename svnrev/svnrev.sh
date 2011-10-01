#! /bin/bash
#

#alte rev auslesen
rev_old=$(awk -FSVN_REV ' $1 ~ /define/ {gsub(" ","",$2); print $2}' svnrev/svnrev.c)

#rev auslesen, die kompiliert wird
rev_new=$(awk -F= '$1 ~ /rev/ {gsub(" ","",$2); print $2}' Makefile)

if [ "$rev_new" != "$rev_old" ]; then
#rev nummer speichern
cat <<EOF > svnrev/svnrev.c
#define SVN_REV $rev_new

int SvnRev()
{
	return SVN_REV;
}
EOF

fi
{
#erstelle meta.xml hbf
hbc_boot=$(awk '-F"' '$1 ~ /hbc_boot/ {print $2}' Makefile)
cat <<EOF > "$hbc_boot"/meta.xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<app version="1">
  <name> Homebrew Filter</name>
  <coder>hamachi-mp</coder>
  <version>r$rev_new</version>
  <no_ios_reload/>
  <short_description>Homebrew Filter / Sorter</short_description>
  <long_description>Dieses HB-App dient zur Sortierung und Kategorisierung Ihrer Homebrews</long_description>
</app>
EOF

#erstelle meta.xml hbf installer
hbc_install=$(awk '-F"' '$1 ~ /hbc_install/ {print $2}' Makefile)
cat <<EOF > "$hbc_install"/meta.xml
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<app version="1">
  <name> Homebrew Filter Installer</name>
  <coder>hamachi-mp</coder>
  <version>r$rev_new</version>
  <no_ios_reload/>
  <short_description>Installer</short_description> 
  <long_description>Installiert den Hombrew Filter</long_description> 
</app>
EOF
}