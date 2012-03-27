#/bin/sh

DATE=20120214
ROOTDIR=`pwd`
mkdir -p $ROOTDIR/Log/Success
export PYRO_COMMON=$ROOTDIR/Pyro_Base/Include/Common
export PYRO_BINARY_DESTINATION=$ROOTDIR/../PyroBuild-$DATE
sync

for BASE in Retry
do
	for a in `ls -1 $ROOTDIR/Pyro_$BASE/` ; do
		if [ $a = "Documentation" ] || [ $a = "Include" ] ; then
				echo $BASE/$a
				cd $ROOTDIR/Pyro_$BASE/$a && make clean < /dev/null >> /dev/null && make dist < /dev/null >> $ROOTDIR/Log/$BASE,$a.log 2>&1 && echo Success: $BASE,$a >> $ROOTDIR/Log/_status.log && mv $ROOTDIR/Log/$BASE,$a.log $ROOTDIR/Log/Success/ && sync
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
															cd $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e/$f && make clean < /dev/null >> /dev/null && make dist < /dev/null >> $ROOTDIR/Log/$BASE,$a,$b,$c,$d,$e,$f.log 2>&1 && echo Success: $BASE,$a,$b,$c,$d,$d,$e,$f >> $ROOTDIR/Log/_status.log && mv $ROOTDIR/Log/$BASE,$a,$b,$c,$d,$e,$f.log $ROOTDIR/Log/Success/ && sync
															cd $ROOTDIR
														fi
													done;
											else
												if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e" ] ; then
													echo $BASE/$a/$b/$c/$d/$e
													cd $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d/$e && make clean < /dev/null >> /dev/null && make dist < /dev/null >> $ROOTDIR/Log/$BASE,$a,$b,$c,$d,$e.log 2>&1 && echo Success: $BASE,$a,$b,$c,$d,$d,$e >> $ROOTDIR/Log/_status.log && mv $ROOTDIR/Log/$BASE,$a,$b,$c,$d,$e.log $ROOTDIR/Log/Success/ && sync
													cd $ROOTDIR
												fi
											fi
										done;
									else
										if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c/$d" ] ; then
											echo $BASE/$a/$b/$c/$d
											cd $ROOTDIR/Pyro_$BASE/$a/$b/$c/$d && make clean < /dev/null >> /dev/null && make dist < /dev/null >> $ROOTDIR/Log/$BASE,$a,$b,$c,$d.log 2>&1 && echo Success: $BASE,$a,$b,$c,$d >> $ROOTDIR/Log/_status.log && mv $ROOTDIR/Log/$BASE,$a,$b,$c,$d.log $ROOTDIR/Log/Success/ && sync
											cd $ROOTDIR
										fi
									fi
								done;
							else
								if [ -d "$ROOTDIR/Pyro_$BASE/$a/$b/$c" ] ; then
									echo $BASE/$a/$b/$c
									cd $ROOTDIR/Pyro_$BASE/$a/$b/$c && make clean < /dev/null >> /dev/null && make dist < /dev/null >> $ROOTDIR/Log/$BASE,$a,$b,$c.log 2>&1 && echo Success: $BASE,$a,$b,$c >> $ROOTDIR/Log/_status.log && mv $ROOTDIR/Log/$BASE,$a,$b,$c.log $ROOTDIR/Log/Success/ && sync
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

mv $ROOTDIR/Log $ROOTDIR/../PyroBuild-$DATE-Log
