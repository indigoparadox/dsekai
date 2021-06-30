#
# Program: Dsekai
#

.c.obj:
	qcl -c -Ic:\msc5\include -DPLATFORM_DOS -DANCIENT_C -W1 -Ze -Zid -AM $*.c

graphics.obj : graphics.c

item.obj : item.c

main.obj : main.c

mobile.obj : mobile.c

tilemap.obj : tilemap.c

topdown.obj : topdown.c

window.obj : window.c

dio.obj : 
	qcl -c -Ic:\msc5\include -DPLATFORM_DOS -DANCIENT_C -W1 -Ze -Zid -AM data\dio.c

drc.obj :
	qcl -c -Ic:\msc5\include -DPLATFORM_DOS -DANCIENT_C -W1 -Ze -Zid -AM data\drc.c

dosg.obj :
	qcl -c -Ic:\msc5\include -DPLATFORM_DOS -DANCIENT_C -W1 -Ze -Zid -AM graphics\dosg.c

dosi.obj : input\dosi.c
	qcl -c -Ic:\msc5\include -DPLATFORM_DOS -DANCIENT_C -W1 -Ze -Zid -AM input\dosi.c

Dsekai.exe : graphics.obj item.obj main.obj mobile.obj tilemap.obj  \
		topdown.obj window.obj dio.obj drc.obj dosg.obj dosi.obj 
	del Dsekai.lnk
	echo graphics.obj+ >>Dsekai.lnk
	echo item.obj+ >>Dsekai.lnk
	echo main.obj+ >>Dsekai.lnk
	echo mobile.obj+ >>Dsekai.lnk
	echo tilemap.obj+ >>Dsekai.lnk
	echo topdown.obj+ >>Dsekai.lnk
	echo window.obj+ >>Dsekai.lnk
	echo dio.obj+ >>Dsekai.lnk
	echo drc.obj+ >>Dsekai.lnk
	echo dosg.obj+ >>Dsekai.lnk
	echo dosi.obj  >>Dsekai.lnk
	echo Dsekai.exe >>Dsekai.lnk
	echo Dsekai.map >>Dsekai.lnk
	link @Dsekai.lnk /NOI $(LDFLAGS);
