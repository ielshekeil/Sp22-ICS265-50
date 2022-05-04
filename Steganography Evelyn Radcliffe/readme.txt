===============================================================================
***ARS OBSCURUM - A SIMPLE STEGANOGRAPHY APPLICATION***
===============================================================================

Usage instructions: 
The program has two modes, embed and extract.

To embed, both the image and payload should be placed in the same folder as
the application. Enter the exact filename of both the image and hidden data, but
do not specify an extension for the output file-- it will automatically be
saved as .png.

To extract, place the image containing your hidden data in the same folder as
the application. Enter the exact filename of the image, but do not specify an
extension for the output file. The original extension is encoded in the hidden
data and will be automatically appended to your specified filename if present.

***

Build environment: This application was built on a Debian-based Linux
environment, and may not compile properly on Windows. 

To build, please use the following command:
gcc -o arsobscurum arsobscurum.c `pkg-config --cflags --libs MagickWand`

Dependencies: This program relies on the package libmagickwand-6.q16-dev.
Please ensure this package is installed before attempting to compile.

Additionally, this program utilizes the POSIX C/Linux system libraries sys/stat.h and sys/types.h and will not compile without access to them.
