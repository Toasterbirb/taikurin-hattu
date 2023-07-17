# .PNG backgrond removal tool
This version of the background removal tool is written in C++ mainly to achieve easier compatibility with Linux based systems

## Compiling instructions
```sh
make
```

## Usage
```sh
asset_cleaner ./exceptions.json ./ASSETS
```
The `exceptions.json` file is a json file containing edge cases. These json files can be found from this repository (or should be at least)


## Third party libraries used
- [json](https://github.com/nlohmann/json)
- [ImageMagick](https://imagemagick.org/)
