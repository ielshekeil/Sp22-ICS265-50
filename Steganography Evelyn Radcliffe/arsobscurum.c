#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>


int embed();
int extract();

int main(void) {
printf("Welcome to Ars Obscurum.\n");


MagickWandGenesis(); 
//In order to use MagickWand, it must be initialized with this command.


int mode;
int status = -1;
char flag;

while (status < 1) {
    printf("Select mode (1 for embed, 2 for extract, anything else to quit):");
    scanf("%d", &mode);
    if(mode == 1) {
        status = embed();
    }
    else if(mode == 2) {
        status = extract();
    }
    else {
        printf("Invalid input. Quit? (y/n)");
        scanf("\n%c", &flag);
        if (flag == 'y') {
            status = 1;
        } else {
            continue;
        }
    }
    if (status < 1) {
        printf("Aborted with errors. Retry? (y/n)");
        scanf("\n%c", &flag);
        if (flag == 'y') {
            continue;
        } else {
            status = 1;
        }
    }
}

//This command cleans up the MagickWand environment after execution.
MagickWandTerminus();

}

int embed() {
    int bitmask = 0x03; //Used to clear the two least significant bits.
    MagickWand *m_wand = NULL;
    PixelWand **px_wand; //Pixel wand arrays for use with the iterators.
    PixelWand **sp_wand; //px is input, sp (secret pixel) is output.
    m_wand = NewMagickWand();
    char filename[50];
    printf("Enter filename for image file: ");
    scanf("%s", filename);
    MagickReadImage(m_wand, filename); //Open input image.
    size_t x = MagickGetImageWidth(m_wand);
    size_t y = MagickGetImageHeight(m_wand);
    size_t maxsize = (x * y) - 5; //the maximum storage size of the image
    double kbsize = (double)maxsize / 1024;
    printf("Image dimensions are %ld by %ld.\n", x, y);
    printf("You can store up to %.2f kb of data in this image.\n", kbsize);
    printf("Enter filename for hidden data: ");
    scanf("\n%s", filename);
    FILE *secret = fopen(filename, "rb");

    struct stat finfo; 
    fstat(fileno(secret), &finfo);
    off_t secretsize = finfo.st_size;
    //Obtain filesize of secret
    if(secretsize < maxsize && secretsize < 0xFFFFFFFF ) {
        //unlikely to trigger, but if secretsize is bigger than the capacity of an int, don't let it be stored
        printf("Success! This data can be stored in your image.\n");
    } else {
        printf("This data is too large for this image. Aborting.\n");
        return -1;    
    }
    char extensionbuffer[5]; //Copy the extension for encoding
    strncpy(extensionbuffer, &filename[strlen(filename)-4], 5); //should copy terminating null
    extensionbuffer[4] = '\0'; //but just in case
    printf("%s\n", extensionbuffer);
    char tempbuffer[5];
    //Clear any characters before the . and pad with nulls.
    int places = 0; //number of places the program had to go forward to find the .
    for(int c = 0; c < 5; c++) {
        if(extensionbuffer[c] == '.') {
            break;
        }
        else {
            places++;
        }
    }
    if (places > 0) {
        strncpy(tempbuffer, &extensionbuffer[places], (5 - places));
        strcpy(extensionbuffer, tempbuffer);
        for(int e = 4; e > (4 - places); e--) {
            extensionbuffer[e] = '\0';
        } //pad with nulls
    }
    MagickWand *sm_wand = NewMagickWand(); //Wand for output file
    //My algorithm relies on the presence of an alpha channel, so this code is meant to enable it if it's missing.
    const MagickBooleanType alphatest = MagickGetImageAlphaChannel(m_wand);
    if(alphatest == MagickFalse) {
        MagickSetImageAlphaChannel(m_wand, ActivateAlphaChannel);
    }
    //This initializes the output to an empty (transparent) canvas the same size as the input.
    MagickSetSize(sm_wand, x, y);
    MagickReadImage(sm_wand,"xc:none");
    printf("Please enter the desired filename for the output image.\nNote: do not include extension.:");
    scanf("\n%s", filename);
    
    //Iterators to obtain pixels from both input and output files.
    PixelIterator *sourcerow = NULL;
    PixelIterator *outrow = NULL;
    sourcerow = NewPixelIterator(m_wand);
    outrow = NewPixelIterator(sm_wand);

    //Status variables for the loop.
    int hasDataLeft = 1;    //Is there data left to write?
    int first = 1;  //Used to control writing the filesize to the first pixel
    int extension = 4; //Used to control writing the extension
    size_t bytes_written = 0; //Tracker for number of bytes of input written
    
    printf("Working. Please wait patiently until ready...\n");
    for (int i = 0; i < y; i++) {
        //Get next row from source and destination images
        px_wand = PixelGetNextIteratorRow(sourcerow, &x);
        sp_wand = PixelGetNextIteratorRow(outrow, &x);
        for(int j = 0; j < x; j++) {
            //Bitflags store seperately if each bit is set
            //As C only works with byte-sized inputs
            //this lets us toggle only the bits we need.
            int bitflags[8] = {0,0,0,0,0,0,0,0};
            char inp; //using a char is a hack to get one-byte inputs
            double red = PixelGetRed(px_wand[j]);
            double blue = PixelGetBlue(px_wand[j]);
            double green = PixelGetGreen(px_wand[j]);
            double alpha = PixelGetAlpha(px_wand[j]);
            //This is really a hack, as MagickWand gives me channel values
            //only between 0 and 1. I convert them into 8-bit values for
            //the purpose of bitwise operations.
            int redi = red * 255;
            int bluei = blue * 255;
            int greeni = green * 255;
            int alphai = alpha * 255;
            int nospam = 0;//this is a debug feature that I'm disabling
            if (first == 1) {
                int smallsize = (int) secretsize;
                //Ints are stored in four bytes, the perfect size for a single pixel. We're compromising a bit by making the first pixel non-concealed. 
                redi = smallsize & (0xFF); //isolate the least significant byte
                greeni = (smallsize >> 8) & (0xFF); //isolate the next most significant byte...
                bluei = (smallsize >> 16) & (0xFF); //etc
                alphai = (smallsize >> 24) & (0xFF);
                //printf("%d %d %d %d", redi, greeni, bluei, alphai);
                first = 0;
            }
            else if (extension > 0) {
                char ext = extensionbuffer[4 - extension];
                //printf("Wrote %c\n", ext);
                bitflags[0] = ext & 0x80; //bitmasks with one bit set
                bitflags[1] = ext & 0x40; //in descending order
                bitflags[2] = ext & 0x20; //This lets me set if each bit
                bitflags[3] = ext & 0x10; //is toggled seperately. 
                bitflags[4] = ext & 0x08;
                bitflags[5] = ext & 0x04;
                bitflags[6] = ext & 0x02;
                bitflags[7] = ext & 0x01;

                redi &= ~bitmask; //unset least significant two bits
                greeni &= ~bitmask; //by bitwise & with inverted bitmask
                bluei &= ~bitmask; //we preserve all bits except the ones
                alphai &= ~bitmask; //we want to unset
                
                //These ifs activate if the flag is non-zero, so it doesn't
                //actually matter what their value as long as it's 0 for false
                //or a positive int for true.
                //Going red -> blue -> green is technically a mistake, but
                //makes it slightly harder for unintended recipients to decode
                if(bitflags[0]) {
                    redi |= 0x02;
                }
                if(bitflags[1]) {
                    redi |= 0x01;
                }
                if(bitflags[2]) {
                    bluei |= 0x02;
                }
                if (bitflags[3]) {
                    bluei |= 0x01;
                }
                if (bitflags[4]) {
                    greeni |= 0x02;
                }
                if (bitflags[5]) {
                    greeni |= 0x01;
                }
                if (bitflags[6]) {
                    alphai |= 0x02;
                }
                if (bitflags[7]) {
                    alphai |= 0x01;
                }
                extension--;
                
            }
            else if (hasDataLeft == 1) {
                hasDataLeft = fread(&inp, sizeof(char), 1, secret);
                //store whether each bit is set, from most significant to least
                bitflags[0] = inp & 0x80; //bitmasks with one bit set
                bitflags[1] = inp & 0x40; //in descending order
                bitflags[2] = inp & 0x20;
                bitflags[3] = inp & 0x10;
                bitflags[4] = inp & 0x08;
                bitflags[5] = inp & 0x04;
                bitflags[6] = inp & 0x02;
                bitflags[7] = inp & 0x01;
                
                redi &= ~bitmask; //unset least significant two bits
                greeni &= ~bitmask;
                bluei &= ~bitmask;
                alphai &= ~bitmask;
                if(nospam) {
                printf("%d %d %d %d\n", redi, greeni, bluei, alphai);
                
                }
                
                //now set the correct bits
                //these ifs will trigger if their corresponding flag is nonzero
                if(bitflags[0]) {
                    redi |= 0x02;
                }
                if(bitflags[1]) {
                    redi |= 0x01;
                }
                if(bitflags[2]) {
                    bluei |= 0x02;
                }
                if (bitflags[3]) {
                    bluei |= 0x01;
                }
                if (bitflags[4]) {
                    greeni |= 0x02;
                }
                if (bitflags[5]) {
                    greeni |= 0x01;
                }
                if (bitflags[6]) {
                    alphai |= 0x02;
                }
                if (bitflags[7]) {
                    alphai |= 0x01;
                }
                if(nospam) {
                printf("%d %d %d %d\n", redi, greeni, bluei, alphai);
                nospam = 0;
                }
                bytes_written++;
            }
            //After we've modified the channel values, convert them back
            //to the double values MagickWand expects.
            //If there's no data left to encode, this will simply copy the
            //input pixel. 
            red = (double) redi / 255;
            blue = (double) bluei / 255;
            green = (double) greeni / 255;
            alpha = (double) alphai / 255;
            //Write modified pixel to output wand.
            PixelSetRed(sp_wand[j], red);
            PixelSetBlue(sp_wand[j], blue);
            PixelSetGreen(sp_wand[j], green);
            PixelSetAlpha(sp_wand[j], alpha);
        }
        //Required to write pixels from iterator to modified image.
        PixelSyncIterator(outrow);
    }
    //Add .png extension for MagickWand to know what we're doing
    //and write modified file out
    strcat(filename, ".png");
    MagickWriteImage(sm_wand, filename);
    printf("Wrote %ld bytes.\n", bytes_written);
    printf("%s written out!\n", filename);

    //Neccessary cleanup.
    sourcerow = DestroyPixelIterator(sourcerow);
    outrow = DestroyPixelIterator(outrow);
    m_wand = DestroyMagickWand(m_wand);
    sm_wand = DestroyMagickWand(sm_wand);
    fclose(secret);
    return 1;
}
int extract() {
    MagickWand *m_wand = NULL;
    PixelWand **px_wand;
    PixelIterator *sourcerow = NULL;
    m_wand = NewMagickWand();

    char filename[50];
    printf("Enter filename for image file: ");
    scanf("%s", filename);

    MagickReadImage(m_wand, filename);
    size_t x = MagickGetImageWidth(m_wand);
    size_t y = MagickGetImageHeight(m_wand);
    sourcerow = NewPixelIterator(m_wand);
    
    printf("Enter filename for output file: (do not add extension) ");
    scanf("%s", filename);

    FILE *out = NULL; // initialize but do not open yet

    //Loop control variables
    int first = 1; //Controls writing filesize to first pixel
    int extension = 4; //Controls writing extension
    size_t bytesencoded = 0; //Counter for bytes encoded (debug)
    char extensionbuffer[5]; //Holds decoded extension
    int opened = 0; //Lets us wait to open file until extension is obtained
    size_t filesize = 5; //dummy value, will be overwritten after first pixel read
    
    for (int i = 0; i < y; i++) {
        //Get next row from source image
        px_wand = PixelGetNextIteratorRow(sourcerow, &x);
        for(int j = 0; j < x; j++) {
            //Bitflags serve same purpose as encoding, simply in reverse
            //Now we're using these to collect the values from the channels
            //and set the proper bits in the output. 
            int bitflags[8] = {0,0,0,0,0,0,0,0};
            char outp = 0; //using a char is a hack to get one-byte inputs
            //Once again, converting from normalized (fractions of 1) channel
            //values to 8-bit channel values
            double red = PixelGetRed(px_wand[j]);
            double blue = PixelGetBlue(px_wand[j]);
            double green = PixelGetGreen(px_wand[j]);
            double alpha = PixelGetAlpha(px_wand[j]);
            int redi = red * 255;
            int bluei = blue * 255;
            int greeni = green * 255;
            int alphai = alpha * 255;
            int nospam = 0;//this is a debug feature that I'm disabling
            if (first == 1) {
                //printf("%d %d %d %d\n", redi, greeni, bluei, alphai);
                int secretsize = 0;
                //The first pixel has the four bytes of an int encoded.
                secretsize |= redi; //red contains least significant
                secretsize |= (greeni << 8); //green next most...
                secretsize |= (bluei << 16); //etc
                secretsize |= (alphai << 24);
                first = 0;
                //We're using ors to flip bits
                filesize = secretsize;
                //printf("%d %ld\n", secretsize, filesize);
            }
            else if (extension > 0) {
                char ext = 0;
                /*
                **bitmask chart from embed**
                bitflags[0] = ext & 0x80; 
                bitflags[1] = ext & 0x40; 
                bitflags[2] = ext & 0x20;
                bitflags[3] = ext & 0x10;
                bitflags[4] = ext & 0x08;
                bitflags[5] = ext & 0x04;
                bitflags[6] = ext & 0x02;
                bitflags[7] = ext & 0x01;
                */

                //very very important to extract the flags in the same order
                //they were encoded in
                //0x02 gets second least significant bit, 0x01 gets least
                bitflags[0] = redi & 0x02;
                bitflags[1] = redi & 0x01;
                bitflags[2] = bluei & 0x02;
                bitflags[3] = bluei & 0x01;
                bitflags[4] = greeni & 0x02;
                bitflags[5] = greeni & 0x01;
                bitflags[6] = alphai & 0x02;
                bitflags[7] = alphai & 0x01;
                
                //And now using ORs with single-bit bitmasks to flip
                //only the bits that were toggled in the input.
                if(bitflags[0]) {
                    ext |= 0x80;
                }
                if(bitflags[1]) {
                    ext |= 0x40;
                }
                if(bitflags[2]) {
                    ext |= 0x20;
                }
                if (bitflags[3]) {
                    ext |= 0x10;
                }
                if (bitflags[4]) {
                    ext |= 0x08;
                }
                if (bitflags[5]) {
                    ext |= 0x04;
                }
                if (bitflags[6]) {
                    ext |= 0x02;
                }
                if (bitflags[7]) {
                    ext |= 0x01;
                }
                extensionbuffer[4 - extension] = ext;
                //rebuild the extension
                extension--;
                
            }
            else if (bytesencoded <= filesize) {
                if(opened == 0) { 
                //This is to delay opening the file until we have the extension
                    extensionbuffer[4] = '\0';
                    strcat(filename, extensionbuffer);
                    out = fopen(filename, "wb");
                    opened = 1;
                } 
                char outp = 0;
                /*
                **bitmask chart from embed**
                bitflags[0] = inp & 0x80; 
                bitflags[1] = inp & 0x40; 
                bitflags[2] = inp & 0x20;
                bitflags[3] = inp & 0x10;
                bitflags[4] = inp & 0x08;
                bitflags[5] = inp & 0x04;
                bitflags[6] = inp & 0x02;
                bitflags[7] = inp & 0x01;
                */

                //very very important to extract the flags in the same order
                //they were encoded in
                //0x02 gets second least significant bit, 0x01 gets least
                //Ands with one bit set leaves only the bit desired in output
                bitflags[0] = redi & 0x02;
                bitflags[1] = redi & 0x01;
                bitflags[2] = bluei & 0x02;
                bitflags[3] = bluei & 0x01;
                bitflags[4] = greeni & 0x02;
                bitflags[5] = greeni & 0x01;
                bitflags[6] = alphai & 0x02;
                bitflags[7] = alphai & 0x01;
                
                //And ors with one bit set enables only the bit desired in output
                if(bitflags[0]) {
                    outp |= 0x80;
                }
                if(bitflags[1]) {
                    outp |= 0x40;
                }
                if(bitflags[2]) {
                    outp |= 0x20;
                }
                if (bitflags[3]) {
                    outp |= 0x10;
                }
                if (bitflags[4]) {
                    outp |= 0x08;
                }
                if (bitflags[5]) {
                    outp |= 0x04;
                }
                if (bitflags[6]) {
                    outp |= 0x02;
                }
                if (bitflags[7]) {
                    outp |= 0x01;
                }
                //Once decoded, we add the newly obtained byte to the output.
                fwrite(&outp, sizeof(char), 1, out);
                bytesencoded++;
            }
        }
    }
        printf("Wrote %ld bytes.\n", bytesencoded);
        printf("File %s written out!\n", filename);
        //Cleanup!
        fclose(out);
        sourcerow = DestroyPixelIterator(sourcerow);
        m_wand = DestroyMagickWand(m_wand);
        return 1;
}


