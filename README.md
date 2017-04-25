# MrFreeze
MrFreeze is an LV2 Plugin designed for [the Mod Duo](https://moddevices.com/) (although it can be also built for other platforms).
This LV2 plugin is an audio effect that provides an infinite sustain pedal for any tonal sound in a way similar as the [Electro Harmonix Freeze pedal] (http://www.ehx.com/products/freeze).
A demo of the effect on the Mod Duo can be seen [here](https://drive.google.com/open?id=0B2q3qYi2cyKQUEZCcmJyWmszMDQ).

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
