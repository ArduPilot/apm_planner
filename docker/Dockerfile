# USAGE:
#
#	# Build apmplanner2 image
#	docker build -t apmplanner2 .
#
#	docker run -v /tmp/.X11-unix:/tmp/.X11-unix \
#		-e DISPLAY=unix$DISPLAY --device /dev/ttyS0:/dev/ttyS0 apmplannner2
FROM ubuntu:15.10
MAINTAINER Rik Bruggink <mail@rikbruggink.nl>
RUN apt-get -y update && apt-get -y upgrade && apt-get -y install wget
RUN wget http://firmware.ardupilot.org/Tools/APMPlanner/apm_planner_2.0.20_ubuntu_wily64.deb
RUN dpkg -i apm_planner_2.0.20_ubuntu_wily64.deb; exit 0
RUN apt-get -f -y install
ENTRYPOINT [ "/usr/bin/apmplanner2"]
