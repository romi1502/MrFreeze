# MrFreeze
Freeze LV2 Plugin

## Build
This project requires the [Eigen library](http://eigen.tuxfamily.org/index.php?title=Main_Page).
The `configure.sh` script retrieve it to the required version for you. Then, before running the `make` command, don't forget to execute the configure script.

## Docker
For those who aren't running on ubuntu, we provide a way to build using docker.
```bash
docker build -t mrfreeze .
docker run -rm -it -v$(pwd):/code mrfreeze make
```
