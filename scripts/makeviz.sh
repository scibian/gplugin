#!/bin/sh

gource \
	--seconds-per-day .1 \
	--auto-skip-seconds .1 \
	--file-idle-time 0 \
	--multi-sampling \
	--highlight-users \
	--stop-at-end \
	--key \
	--title "History of GPlugin" \
	--font-size 26 \
	-1280x720 \
	--max-files 0 \
	--hide filenames,mouse,progress \
	--output-framerate 30 \
	-o - \
| ffmpeg \
	-y \
	-r 30 \
	-f image2pipe \
	-vcodec ppm \
	-i - \
	-vcodec libx264 \
	-preset ultrafast \
	-pix_fmt yuv420p \
	-crf 1 \
	-threads 0 \
	-bf 0 \
	gplugin.mp4

