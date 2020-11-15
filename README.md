# steg

---
Article : [Steganography, Hide Text in Image](http://jnjenga.com/articles/steganography-hide-text-in-image/)

Steg is a steganography implementation done using C. Steganography is the art of hiding information inside other files, in this case inside an image.

 Can you see any difference ?

|        Original         |              Encrypted              |
| :---------------------: | :---------------------------------: |
| ![Original](test-files/original.bmp) | ![Encrypted](generated-files/enc-output.bmp) |

The second image has the English version of the Kenyan national anthem hidden inside it.

## How Build

`make build` - Requires GCC compiler 

or

Compile the `steg.c` file using any C compiler that  supports C99.

## Usage

`steg [mode] [type] [input_file] [out_file] [plain_text]`

`steg [mode] [type] [input_file] [out_file]`

**Mode** : Encrypting or decrypting

- `-e` - encrypting
- `-d` - decrypting

**Type** : Type of data we are hiding, text or image data

- `-t` - text, hide text data in image
- ~~`-i` - hide image inside another image~~

> Only supports ARGB bitmap file format at the moment

### Usage Example

Encrypting

`steg -e -t input.bmp output.bmp plain.txt`

Decrypting

`steg -d -t input.bmp output.txt`


## License

Check license readme

