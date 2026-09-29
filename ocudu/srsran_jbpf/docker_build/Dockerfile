FROM mcr.microsoft.com/mirror/docker/library/ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]

RUN echo "*** Installing packages"
RUN apt update --fix-missing
RUN apt install -y libboost-dev git libboost-program-options-dev libboost-filesystem-dev

COPY ./out /out
ENTRYPOINT ["/bin/bash"]
