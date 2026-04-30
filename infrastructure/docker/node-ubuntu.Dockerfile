FROM ubuntu:22.04

ARG DEBIAN_FRONTEND=noninteractive
ENV container=docker

RUN apt-get update && apt-get install -y --no-install-recommends \
    systemd \
    systemd-sysv \
    dbus \
    dbus-user-session \
    openssh-server \
    sudo \
    python3 \
    python3-apt \
    iproute2 \
    iputils-ping \
    curl \
    ca-certificates \
    vim-tiny \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

RUN mkdir /var/run/sshd \
    && echo 'root:password' | chpasswd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

RUN useradd -m -s /bin/bash -G sudo user \
    && echo 'user:password' | chpasswd \
    && echo 'user ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

RUN cd /lib/systemd/system/sysinit.target.wants/ \
    && ls | grep -v systemd-tmpfiles-setup | xargs rm -f \
    && rm -f /lib/systemd/system/multi-user.target.wants/* \
    && rm -f /etc/systemd/system/*.wants/* \
    && rm -f /lib/systemd/system/local-fs.target.wants/* \
    && rm -f /lib/systemd/system/sockets.target.wants/*udev* \
    && rm -f /lib/systemd/system/sockets.target.wants/*initctl* \
    && rm -f /lib/systemd/system/basic.target.wants/* \
    && rm -f /lib/systemd/system/anaconda.target.wants/* \
    && systemctl enable ssh

VOLUME [ "/sys/fs/cgroup" ]
STOPSIGNAL SIGRTMIN+3
CMD ["/lib/systemd/systemd"]