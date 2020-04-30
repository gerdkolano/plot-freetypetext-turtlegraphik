LDLIBS=-lnetpbm -lm

apfelmann: apfelmann.o
	g++    -o apfelmann apfelmann.o

apfelmann.o: apfelmann.cpp
	g++ -c -o apfelmann.o apfelmann.cpp

koch: koch.o plot3.o turtle.o abbild.o turtle.h
koch.o : koch.c turtle.h
plot3.o pam.o: von5nach5.h turtle.h
turtle.o: plot3.o abbild.o turtle.h

test-001: apfelmann
	./apfelmann -l -1.8 -r -1.74 -t 160

test-002: apfelmann
	./apfelmann -l -2.0 -r 0.5 -t 160

#
test-007: koch pam
	./koch -x -1.8 -y -1.8 -h2 -w2 | ./pam -x600 -y600 2>/dev/null > /tmp/koch.pnm ; eog -n /tmp/koch.pnm

# f=/tmp/4; sudo make -C /heim/hanno/c/framebuffer/turtle test-001 2>&1 | tee ${f}4 | perl -pne 's/[0-9A-F]{7,8}/0000000/g' > $f  && diff --context --ignore-matching-lines=g++ /tmp/1 $f

turtle-001-nur-plotpnm: turtle-001-nur-plotpnm.o
	g++ -o turtle-001-nur-plotpnm turtle-001-nur-plotpnm.o

turtle-001-nur-plotpnm.o: turtle-001-nur-plotpnm.cpp
	g++ -c -o turtle-001-nur-plotpnm.o turtle-001-nur-plotpnm.cpp

turtle-009-nur-plotpnm: turtle-009-nur-plotpnm.o
	g++ --std=c++11 -o turtle-009-nur-plotpnm turtle-009-nur-plotpnm.o -lfreetype

turtle-009-nur-plotpnm.o: turtle-009-nur-plotpnm.cpp
	g++ --std=c++11 -c -o turtle-009-nur-plotpnm.o turtle-009-nur-plotpnm.cpp  -I/usr/include/freetype2

turtle-008-nur-plotpnm: turtle-008-nur-plotpnm.o
	g++ -o turtle-008-nur-plotpnm turtle-008-nur-plotpnm.o

turtle-008-nur-plotpnm.o: turtle-008-nur-plotpnm.cpp
	g++ -c -o turtle-008-nur-plotpnm.o turtle-008-nur-plotpnm.cpp

freetype.o: freetype.c
	g++ -c -o freetype.o freetype.c -I/usr/include/freetype2

freetype: freetype.o
	g++ -o freetype freetype.o -lfreetype

freetype-example: freetype
	./freetype  /usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-R.ttf ABCdefäölüßghixyzLOKj | less -S

SHELL=/bin/bash
Bush-31a-film:
	for i in /data7/tmp/Bush-31a/*.pnm; do q=$${i}; z=$${i/.pnm/.jpg}; convert $${q} $${z}; done
	mencoder "mf:///data7/tmp/Bush-31a/*.jpg" -fps 25 -vf scale=1920:1080 -ovc lavc -lavcopts vcodec=mjpeg -o /data7/tmp/Bush-31a-avi-25fps.avi

sierpinski-film:
	for i in /data7/tmp/sierpinski/*.pnm; do q=$${i}; z=$${i/.pnm/.jpg}; convert $${q} $${z}; done
	mencoder "mf:///data7/tmp/sierpinski/*.jpg" -fps 25 -vf scale=1920:1080 -ovc lavc -lavcopts vcodec=mjpeg -o /tmp/turtle-08-sierpinski-avi-25fps.avi
	# mencoder "mf:///data7/tmp/sierpinski/*8.jpg" -fps 25 -vf scale=1920:1080 -ovc lavc -lavcopts vcodec=mjpeg -o /tmp/turtle-08-sierpinski-008-25fps.avi
	# mencoder "mf:///data7/tmp/sierpinski/*[38].jpg" -fps 25 -vf scale=1920:1080 -ovc lavc -lavcopts vcodec=mjpeg -o /tmp/turtle-08-sierpinski-038-25fps.avi
	# AVFrame.format AVFrame.width or height is not set

%.avi: %.jpg
	# ohne sound
	mencoder "mf://*.jpg" -fps 4 -vf scale=595:842 \
	         -ovc lavc -lavcopts vcodec=mjpeg -o $@
	# mit sound
	mencoder -audiofile saege.mp2 -noskip -fps 25 -mf fps=25 -oac lavc -ovc lavc -lavcopts vcodec=mpeg4:acodec=mp2:autoaspect -v
%.flv: %.jpg
	# Ohne Sound
	mencoder -of lavf -oac mp3lame -lameopts abr:br=56 -srate 22050 -ovc lavc -lavcopts vcodec=flv:vbitrate=500:mbd=2:mv0:trell:

