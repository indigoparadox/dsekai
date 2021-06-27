#
# Program: Dsekai
#

.c.obj:
	qcl -c -Ic:\msc5\include -DPLATFORM_DOS -DANCIENT_C -W1 -Ze -Zid -AM $*.c

graphics.obj : src\graphics.c

item.obj : src\item.c

main.obj : src\main.c

mobile.obj : src\mobile.c

tilemap.obj : src\tilemap.c

topdown.obj : src\topdown.c

window.obj : src\window.c

dio.obj : src\data\dio.c

drc.obj : src\data\drc.c

dosg.obj : src\graphics\dosg.c

dosi.obj : src\input\dosi.c

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
