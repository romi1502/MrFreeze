FROM ubuntu:16.04

RUN apt-get update -y
RUN apt-get install -y libfftw3-dev build-essential lv2-dev

RUN mkdir -p /code
WORKDIR /code
