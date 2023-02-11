# GG2IMG

Inserts a file built with [Golden Gate](http://goldengate.gitlab.io/) into a disk image file (.po), preserving resource information. This works on Windows only, and requires the NTFS file system, per [Golden Gate's requirements](http://goldengate.gitlab.io/manual/#file-systems).

## Build
Builds with Visual Studio 2022.  Earlier versions may work as well.

## Usage
`gg2img [IIgs file built with Golden Gate] [disk image file]`

## Dependencies
GG2IMG uses the following libraries, taken from [CiderPress](https://github.com/fadden/ciderpress). Please see the `license.txt` file in the `libs` directory for details.

- DiskImg
- NufxLib
- Zlib

## Links to Similar Tools
- [Cadius](https://www.brutaldeluxe.fr/products/crossdevtools/cadius/) (expects resource forks to be in separate files, so not entirely compatible with Golden Gate)
- [Cadius with improvements](https://github.com/mach-kernel/cadius) (and Linux support)
