# Ubuntu 24.04 LTS Jammy
# APM Planner 2 build env
#
# - build using the follow arguments to setup the docker with common user_id/gid for file permissions.
# docker build --build-arg USERNAME=$(whoami) --build-arg USER_UID=$(id -u) --build-arg USER_GID=$(id -g) -t qt515-user .
#
FROM ubuntu:24.04

# Create a non-root user
ARG USERNAME=developer
ARG USER_UID=1000
ARG USER_GID=$USER_GID

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      build-essential \
      git \
      wget \
      curl \
      ca-certificates \
      python3 \
      perl \
      libgl1-mesa-dev \
      libx11-dev \
      libxext-dev \
      libxfixes-dev \
      libxi-dev \
      libxrender-dev \
      libxrandr-dev \
      libxcursor-dev \
      libxinerama-dev \
      libxss-dev \
      libxv-dev \
      libxkbcommon-x11-0 \
      libdbus-1-dev \
      libfontconfig1-dev \
      libfreetype6-dev \
      libicu-dev \
      libpng-dev \
      libjpeg-dev \
      libssl-dev \
      libsqlite3-dev \
      libxml2-dev \
      libxslt1-dev \
      && rm -rf /var/lib/apt/lists/*

# Create group if needed, then user with correct UID/GID
RUN set -eux; \
    echo "USER_GID: $USER_GID"; \
    if getent group $USER_GID; then \
      groupname="$(getent group $USER_GID | cut -d: -f1 | awk '{print $1}')"; \
      echo "Group exists: $groupname"; \
    else \
      groupname=$USERNAME; groupadd --gid $USER_GID $groupname; \
      echo "Created group: $groupname"; \
    fi; \
    echo "About to create user: $USERNAME with group $groupname"; \
    eval useradd --uid $USER_UID --gid $groupname -m $USERNAME

RUN mkdir -p /home/linuxbrew && chown $USERNAME:$USERNAME /home/linuxbrew

USER $USERNAME
WORKDIR /home/$USERNAME

# Install Homebrew non-interactively as the non-root user
RUN /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Set Homebrew in the path for future RUN commands and shells
ENV PATH="/home/linuxbrew/.linuxbrew/bin:/home/linuxbrew/.linuxbrew/sbin:${PATH}"
ENV HOMEBREW_NO_AUTO_UPDATE=1

# Optionally, verify brew install
RUN brew --version

RUN if [ ! -z "$USE_BREW" ]; then ulimit -n 8192; brew install qt@5 && brew link qt@5 --force; fi

ENV QT_VERSION=5.15.16
ENV QTDIR=/home/$USERNAME/Qt${QT_VERSION}
ENV PATH=$QTDIR/bin:$PATH

RUN set -eux; echo "working dir $(pwd)"
RUN wget https://download.qt.io/archive/qt/5.15/${QT_VERSION}/single/qt-everywhere-opensource-src-${QT_VERSION}.tar.xz && \
    tar -xf qt-everywhere-opensource-src-${QT_VERSION}.tar.xz && \
    rm qt-everywhere-opensource-src-${QT_VERSION}.tar.xz && \
    cd qt-everywhere-src-${QT_VERSION} && \
    ./configure -prefix $QTDIR -opensource -confirm-license -nomake tests -nomake examples && \
    make -j$(nproc) && \
    make install && \
    cd .. && rm -rf qt-everywhere-src-${QT_VERSION}

# Set environment variables for user space Qt
ENV QTDIR=$QTDIR
ENV PATH=$QTDIR/bin:$PATH
ENV LD_LIBRARY_PATH=$QTDIR/lib:$LD_LIBRARY_PATH
ENV PKG_CONFIG_PATH=$QTDIR/lib/pkgconfig:$PKG_CONFIG_PATH

WORKDIR /home/$USERNAME/dev_src/apm_planner

CMD ["/bin/bash"]
