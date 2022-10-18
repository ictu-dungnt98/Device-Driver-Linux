FROM ubuntu:20.04
ARG UID=1000

ENV TZ=Asia/Dubai
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt update
RUN apt install -y tzdata

RUN apt-get update -y && apt-get install -y \
        build-essential \
        vim curl git cmake make wget \
        cpio python unzip rsync bc \
        autopoint automake autotools-dev \
        libtool libncurses-dev libxml-parser-perl \
        lib32z1 lib32ncurses5-dev lib32stdc++6 libssl-dev \
        libssl-dev sudo fakeroot pkg-config \
        protobuf-c-compiler libgcrypt-dev gperf texinfo

# install python to build the swap partition
RUN apt-get install -y python python3-pip
RUN pip install crypto pycryptodome

# setup date
RUN apt-get install -y locales
RUN sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen && \
        echo 'LANG="en_US.UTF-8"'>/etc/default/locale && \
        dpkg-reconfigure --frontend=noninteractive locales && \
        update-locale LANG=en_US.UTF-8

ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8

RUN useradd -m -G sudo -u ${UID} -p qf0mfgJ9rQeLY dungnt98
COPY .bashrc .profile /home/dungnt98/
#COPY aarch64-none-elf-4.9 /opt/aarch64-none-elf-4.9/
#COPY gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu /opt/gcc-linaro-7.2.1-2017.11-x86_64_aarch64-linux-gnu/
RUN chown -R dungnt98:dungnt98 /opt

USER dungnt98
WORKDIR /home/dungnt98/work/
CMD "/bin/bash"
