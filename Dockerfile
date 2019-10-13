FROM ubuntu:18.04

RUN apt-get update
RUN apt-get install vim -y
RUN apt-get install nmap -y
RUN apt-get install -y git
RUN apt-get install -y zsh
RUN apt-get install -y wget
RUN apt-get install -y gcc
RUN wget https://github.com/robbyrussell/oh-my-zsh/raw/master/tools/install.sh -O - | zsh || true
RUN chsh -s $(which zsh)

RUN apt-get -y install sudo

RUN useradd -m docker && echo "docker:docker" | chpasswd && adduser docker sudo
USER docker

COPY . /project

CMD zsh