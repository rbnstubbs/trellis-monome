#!/bin/sh

INST=`ls /System/Library/Extensions/ | grep FTDI`

if [ "$INST" != "" ]; then
	echo "FTDI installed:         ok"
else                             
	echo "FTDI installed:         NOT OK -- install FTDI driver."
	echo $INST
fi

FTDI=`kextstat | grep FTDI`

if [ "$FTDI" != "" ]; then
	echo "FTDI running:           ok"
else                              
	echo "FTDI running:           NOT OK -- either attach device or re-enable FTDI driver. run on-ftdi.command"
fi

INSTALLED=`which serialoscd`

if [ "$INSTALLED" == "/usr/local/bin/serialoscd" ]; then
	echo "SERIALOSC installed:    ok"
else
	echo "SERIALOSC installed:    NOT OK -- install serialosc."
fi


RUNNING=`launchctl list | grep serialosc`

if [ "$RUNNING" != "" ]; then
	echo "SERIALOSC running:      ok"
else
	echo "SERIALOSC running:      NOT OK -- serialosc not running. run on-serialosc.command"
fi


#MAXPAT=`ls /Applications/Max6/patches/serialosc.maxpat`

if [ `ls /Applications/Max6/patches/serialosc.maxpat` != "" ]; then
	echo "MAXPAT installed:       ok (max 6)"
elif [ `ls /Applications/Max5/patches/serialosc.maxpat` != "" ]; then
	echo "MAXPAT installed:       ok (max 5)"
elif [ `ls /Applications/Max6.1/patches/serialosc.maxpat` != "" ]; then
	echo "MAXPAT installed:       ok (max 6.1)"
else
	echo "MAXPAT installed:       NOT OK -- copy serialosc.maxpat to /Applications/Max6/patches (correct for your version)"
fi
