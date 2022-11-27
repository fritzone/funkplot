# map /funkplot to /home/fld/work/p/funkplot
# sudo docker run -d --name funkplot --privileged -v /home/ferencd/work/p/funkplot/:/funkplot/ funkplot:latest

FROM ubuntu:20.04

ENV TZ 'Europe/Oslo'
ENV DEBIAN_FRONTEND=noninteractive

RUN DEBIAN_FRONTEND=noninteractive TZ=Europe/Oslo echo $TZ > /etc/timezone && \
  		apt-get update && apt-get install -y apt-utils tzdata && \
  		rm /etc/localtime && \
  		ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && \
  		dpkg-reconfigure -f noninteractive tzdata && \
  		apt-get clean && \
  		apt-get install debconf-utils &&\
  		apt-get install -y keyboard-configuration &&\
        apt-get install -y wget equivs devscripts python-is-python3 debhelper qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev qtscript5-dev \
		libqt5opengl5-dev libqt5x11extras5 libqt5x11extras5-dev qtxmlpatterns5-dev-tools \
		libqt5webkit5-dev qtwebengine5-dev libqt5svg5-dev qttools5-dev libqt5help5 \
		qtbase5-private-dev cmake git pkg-config build-essential qtwebengine5-dev \
		libpython3-dev mesa-common-dev python3 latex2rtf imagemagick texlive-latex-extra

# make install DESTDIR=AppDir
# wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
# chmod a+x linuxdeployqt-continuous-x86_64.AppImage