# rtl_gnss

> Disclaimer: This project is not yet functional, contributions are always welcome to accelerate development towards the first fix using rtl_gnss!

Turn your [RTL-SDR](https://osmocom.org/projects/rtl-sdr/wiki) into a [GNSS](https://en.wikipedia.org/wiki/GNSS) receiver with rtl_gnss!

## Possible Modes

Due to the RTL-SDR's limited bandwidth, shown below are the theoretically possible configurations for single-frequency GNSS reception when tuned to a given frequency.

### 1575.42 MHz

- [ ] GPS L1 C/A
- [ ] GPS L1C
- [ ] Galileo E1 OS
- [ ] GLONASS CDMA
- [ ] BeiDou B1C
- [ ] QZSS L1 C/A
- [ ] QZSS L1C
- [ ] QZSS SAIF

### SBAS

It may also be possible to recieve SBAS corrections in the same bandwidth.

- 1574.42 MHz WAAS
- 1575.42 MHz EGNOS
- 1575.42 MHz MSAS
- 1575.42 MHz GAGAN
- 1575.42 MHz BDSBAS
- 1575.42 MHz SouthPAN
- SACCSA?
- 1575.42? MHz ASECNA

## Unsorted (TODO)

- [x] GPS L2C
- [ ] GLONASS Partial FDMA
- [ ] BeiDou B1I
- [ ] BeiDou B2I?
- [ ] QZSS L2C
