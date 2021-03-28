FROM vookimedlo/ubuntu-qt:5.12_gcc_xenial

RUN apt-get update && apt-get install -y qt512svg qt512serialport zlib1g-dev libsdl2-dev libasound2-dev libsndfile1-dev flite1-dev libfuse2 appstream

COPY ./apmplanner2.desktop /home/build/appimage/
COPY ./apmplanner2.appdata.xml /home/build/appimage/
COPY ./buildAppimage.sh /home/build/appimage/

ENV PATH "$PATH:/opt/qt512/bin"

CMD ["/home/build/appimage/buildAppimage.sh"]
