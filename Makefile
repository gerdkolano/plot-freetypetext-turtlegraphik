turtle-plot-pnm: turtle-plot-pnm.o
	g++ --std=c++11 -o turtle-plot-pnm turtle-plot-pnm.o -lfreetype

turtle-plot-pnm.o: turtle-plot-pnm.cpp
	g++ --std=c++11 -c -o turtle-plot-pnm.o turtle-plot-pnm.cpp  -I/usr/include/freetype2

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

# make -C /heim/hanno/c/framebuffer/plot-freetypetext-turtlegraphik/ turtle-plot-pnm 2>/tmp/e && ./turtle-plot-pnm -t 2 2> /tmp/1
# make -C /home/hanno/c/framebuffer/plot-freetypetext-turtlegraphik/ turtle-plot-pnm 2>/tmp/e && ./turtle-plot-pnm -t 2 2> /tmp/1
