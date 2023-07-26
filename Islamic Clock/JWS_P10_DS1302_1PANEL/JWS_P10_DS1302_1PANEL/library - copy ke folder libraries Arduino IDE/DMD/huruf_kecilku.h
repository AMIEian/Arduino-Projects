/*
 *
 * System5x7
 *
 *
 * File Name           : huruf_kecilku.h
 * Date                : 28 Oct 2008
 * Font size in bytes  : 470
 * Font width          : 5
 * Font height         : 7
 * Font first char     : 32
 * Font last char      : 127
 * Font used chars     : 94
 *
 * The font data are defined as
 *
 * struct _FONT_ {
 *     uint16_t   font_Size_in_Bytes_over_all_included_Size_it_self;
 *     uint8_t    font_Width_in_Pixel_for_fixed_drawing;
 *     uint8_t    font_Height_in_Pixel_for_all_characters;
 *     unit8_t    font_First_Char;
 *     uint8_t    font_Char_Count;
 *
 *     uint8_t    font_Char_Widths[font_Last_Char - font_First_Char +1];
 *                  // for each character the separate width in pixels,
 *                  // characters < 128 have an implicit virtual right empty row
 *
 *     uint8_t    font_data[];
 *                  // bit field of all characters
 */

#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef huruf_kecilku_H
#define huruf_kecilku_H

#define huruf_kecilku_WIDTH 5
#define huruf_kecilku_HEIGHT 7

/*
 * added to allow fontname to match header file name. 
 * as well as keep the old name for backward compability
 */

#define huruf_kecilku huruf_kecilku

const static uint8_t huruf_kecilku[] PROGMEM = {
    0x0, 0x0, // size of zero indicates fixed width font, actual length is width * height
    0x05, // width
    0x07, // height
    0x20, // first char
    0x60, // char count
    
    // Fixed width; char width table not used !!!!
    
    // font data
    
	  0x00, 0x00, 0x00, 0x00, 0x00,// (space)
	0x00, 0x00, 0x5F, 0x00, 0x00,// !
	0x00, 0x07, 0x00, 0x07, 0x00,// "
	0x14, 0x7F, 0x14, 0x7F, 0x14,// #
	0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
	0x23, 0x13, 0x08, 0x64, 0x62,// %
	0x36, 0x49, 0x55, 0x22, 0x50,// &
	0x00, 0x05, 0x03, 0x00, 0x00,// '
	0x00, 0x1C, 0x22, 0x41, 0x00,// (
	0x00, 0x41, 0x22, 0x1C, 0x00,// )
	0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
	0x08, 0x08, 0x3E, 0x08, 0x08,// +
	0x00, 0x50, 0x30, 0x00, 0x00,// ,
	0x08, 0x08, 0x08, 0x08, 0x08,// -
	0x00, 0x60, 0x60, 0x00, 0x00,// .
	0x20, 0x10, 0x08, 0x04, 0x02,// /


///////////////////////////////////
        0x04, 0x3F, 0x21, 0x21, 0x3F,            // Code for char 0
        0x04, 0x00, 0x21, 0x3F, 0x20,            // Code for char 1
        0x04, 0x39, 0x29, 0x29, 0x2F,            // Code for char 2
        0x04, 0x21, 0x29, 0x29, 0x3F,            // Code for char 3
        0x04, 0x0F, 0x08, 0x08, 0x3F,            // Code for char 4
        0x04, 0x2F, 0x29, 0x29, 0x39,            // Code for char 5
        0x04, 0x3F, 0x25, 0x25, 0x3D,            // Code for char 6
        0x04, 0x01, 0x01, 0x01, 0x3F,            // Code for char 7
        0x04, 0x3F, 0x29, 0x29, 0x3F,            // Code for char 8
        0x04, 0x2F, 0x29, 0x29, 0x3F,            // Code for char 9



//////////////////////////////////////////
	       0x02, 0x00, 0x12, 0x00, 0x00,            // Code for char :
        0x03, 0x00, 0x12, 0x12, 0x00,            // Code for char ;
        0x02, 0x00, 0x12, 0x00, 0x00,            // Code for char <
        0x04, 0x0A, 0x0A, 0x0A, 0x0A,            // Code for char =
        0x02, 0x00, 0x12, 0x00, 0x00,            // Code for char >
        0x01, 0x00, 0x00, 0x00, 0x00,            // Code for char ?
        0x04, 0x3E, 0x23, 0x21, 0x3F,            // Code for char @
	0x04, 0x3F, 0x09, 0x09, 0x3F,            // Code for char A
        0x04, 0x3F, 0x29, 0x29, 0x37,            // Code for char B
        0x04, 0x3F, 0x21, 0x21, 0x21,            // Code for char C
        0x04, 0x3F, 0x21, 0x21, 0x1E,            // Code for char D
        0x04, 0x3F, 0x29, 0x29, 0x21,            // Code for char E
        0x04, 0x3F, 0x09, 0x09, 0x01,            // Code for char F
        0x04, 0x3F, 0x21, 0x29, 0x39,            // Code for char G
        0x04, 0x3F, 0x08, 0x08, 0x3F,            // Code for char H
        0x02, 0x00, 0x3F, 0x00, 0x00,            // Code for char I
        0x04, 0x38, 0x20, 0x20, 0x3F,            // Code for char J
        0x04, 0x3F, 0x0C, 0x12, 0x21,            // Code for char K
        0x04, 0x3F, 0x20, 0x20, 0x20,            // Code for char L
        0x04, 0x3F, 0x0E, 0x0E, 0x3F,            // Code for char M
        0x04, 0x3F, 0x01, 0x01, 0x3F,            // Code for char N
        0x04, 0x3F, 0x21, 0x21, 0x3F,            // Code for char O
        0x04, 0x3F, 0x09, 0x09, 0x0F,            // Code for char P
        0x04, 0x3F, 0x29, 0x31, 0x3F,            // Code for char Q
        0x04, 0x3F, 0x09, 0x19, 0x2F,            // Code for char R
        0x04, 0x2F, 0x29, 0x29, 0x39,            // Code for char S
        0x03, 0x01, 0x3F, 0x01, 0x00,            // Code for char T
        0x04, 0x3F, 0x20, 0x20, 0x3F,            // Code for char U
        0x04, 0x0F, 0x30, 0x30, 0x0F,            // Code for char V
        0x04, 0x3F, 0x3C, 0x38, 0x3F,            // Code for char W
        0x04, 0x33, 0x0C, 0x0C, 0x3B,            // Code for char X
        0x04, 0x2F, 0x28, 0x28, 0x3F,            // Code for char Y
        0x04, 0x31, 0x29, 0x25, 0x23,            // Code for char Z
	0x00, 0x00, 0x7F, 0x41, 0x41,// [
	0x02, 0x04, 0x08, 0x10, 0x20,// "\"
	0x41, 0x41, 0x7F, 0x00, 0x00,// ]
	0x04, 0x02, 0x01, 0x02, 0x04,// ^
	0x40, 0x40, 0x40, 0x40, 0x40,// _
	0x00, 0x01, 0x02, 0x04, 0x00,// `
	0x04, 0x3D, 0x25, 0x25, 0x3F,            // Code for char a
        0x04, 0x3F, 0x24, 0x24, 0x3C,            // Code for char b
        0x04, 0x3F, 0x21, 0x21, 0x21,            // Code for char c
        0x04, 0x3C, 0x24, 0x24, 0x3F,            // Code for char d
        0x04, 0x3F, 0x29, 0x29, 0x2F,            // Code for char e
        0x04, 0x3F, 0x05, 0x05, 0x01,            // Code for char f
        0x04, 0x2F, 0x29, 0x29, 0x3F,            // Code for char g
        0x04, 0x3F, 0x04, 0x04, 0x3C,            // Code for char h
        0x02, 0x00, 0x3D, 0x00, 0x00,            // Code for char i
        0x04, 0x30, 0x20, 0x20, 0x3F,            // Code for char j
        0x04, 0x3F, 0x0C, 0x12, 0x21,            // Code for char k
        0x04, 0x3F, 0x20, 0x20, 0x20,            // Code for char l
        0x04, 0x3F, 0x07, 0x07, 0x3F,            // Code for char m
        0x04, 0x3F, 0x01, 0x01, 0x3F,            // Code for char n
        0x04, 0x3F, 0x21, 0x21, 0x3F,            // Code for char o
        0x04, 0x3F, 0x09, 0x09, 0x0F,            // Code for char p
        0x04, 0x0F, 0x09, 0x09, 0x3F,            // Code for char q
        0x04, 0x3F, 0x02, 0x01, 0x01,            // Code for char r
        0x04, 0x2F, 0x29, 0x29, 0x39,            // Code for char s
        0x04, 0x04, 0x3F, 0x24, 0x24,            // Code for char t
        0x04, 0x3F, 0x20, 0x20, 0x3F,            // Code for char u
        0x04, 0x0F, 0x30, 0x30, 0x0F,            // Code for char v
        0x04, 0x3F, 0x38, 0x38, 0x3F,            // Code for char w
        0x04, 0x33, 0x0C, 0x0C, 0x33,            // Code for char x
        0x04, 0x2F, 0x28, 0x28, 0x3F,            // Code for char y
        0x04, 0x31, 0x29, 0x25, 0x23,            // Code for char z
	0x00, 0x08, 0x36, 0x41, 0x00,// {
	0x00, 0x00, 0x7F, 0x00, 0x00,// |
	0x00, 0x41, 0x36, 0x08, 0x00,// }
	0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
	0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
	
    
};

#endif
