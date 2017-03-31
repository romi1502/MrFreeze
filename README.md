# MrFreeze
Freeze LV2 Plugin

## Dependencies

### Eigen

This project requires the [Eigen library](http://eigen.tuxfamily.org/index.php?title=Main_Page) ver 3.3.3.
The `configure.sh` script is made to retrieve it for you. Then, before running the `make` command, don't forget to execute the configure script.
```bash
./configure.sh
```

### Other

```bash
sudo apt-get install libfftw3-dev build-essential lv2-dev pkg-config
```

## Docker
For those who aren't running ubuntu, we provide a way to build using docker.
```bash
docker build -t mrfreeze .
docker run --rm -v$(pwd):/code mrfreeze make
```
