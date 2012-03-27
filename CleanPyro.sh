#/bin/sh

ROOTDIR=`pwd`
export PYRO_COMMON=/boot/Include/Common
sync

for BASE in Base Extra Retry
do
	for a in `ls -1 $ROOTDIR/Pyro_$BASE/` ; do
		if [ $a = "Documentation" ] || [ $a = "Include" ] ; then
				echo $BASE/$a
				cd $ROOTDIR/Pyro_$BASE/$a && make clean < /dev/null >> /dev/null && sync
				cd $ROOTDIR
		else
			if [ -d "$ROOTDIR/Pyro_$BASE/$a" ] ; then
				for b in `ls -1 $ROOTDIR/Pyro_$BASE/$a/` ; do
					if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b" ] ; then
						for c in `ls -1 $ROOTDIR/Pyro_$BASE/$a/$b/` ; do
							if [ $c = "media" ] || [ $c = "translators" ] || [ $c = "appserver" ] || [ $c = "Dock" ] || [ $c = "ColdFish" ] || [ $c = "drivers" ] || [ $c = "fs" ] ; then
								for d in `ls -1 $ROOTDIR/Pyro_$BASE/$a/$b/$c/` ; do
									if [ $c = "drivers" ] || [ $d = "drivers" ] || [ $d = dock_plugins ] || [ $d = ColdFish_Plugins ]; then
										for e in `ls -1 $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/` ; do
											if [ $d = "drivers" ] ; then
													for f in `ls -1 $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e/` ; do
														if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e/$f" ] ; then
															echo $BASE/$a/$b/$c/$d/$e/$f
															cd $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e/$f && make clean < /dev/null >> /dev/null && sync
															cd $ROOTDIR
														fi
													done;
											else
												if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e" ] ; then
													echo $BASE/$a/$b/$c/$d/$e
													cd $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e && make clean < /dev/null >> /dev/null && sync
													cd $ROOTDIR
												fi
											fi
										done;
									else
										if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c/$d" ] ; then
											echo $BASE/$a/$b/$c/$d
											cd $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d && make clean < /dev/null >> /dev/null && sync
											cd $ROOTDIR
										fi
									fi
								done;
							else
								if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c" ] ; then
									echo $BASE/$a/$b/$c
									cd $ROOTDIR/Pyro_$BASE/$a/$b/$c && make clean < /dev/null >> /dev/null && sync
									cd $ROOTDIR
								fi
							fi
						done;
					fi
				done;
			fi
		fi
	done;
done;
