/*
 * NuFX archive manipulation library
 * Copyright (C) 2014 by Andy McFadden, All Rights Reserved.
 * This is free software; you can redistribute it and/or modify it under the
 * terms of the BSD License, see the file COPYING-LIB.
 *
 * Miscellaneous NufxLib utility functions.
 */
#include "NufxLibPriv.h"

/*
 * Convert Mac OS Roman to Unicode.  Mapping comes from:
 *
 * http://www.unicode.org/Public/MAPPINGS/VENDORS/APPLE/ROMAN.TXT
 *
 * We use the "Control Pictures" block for the control characters
 * (0x00-0x1f, 0x7f --> 0x2400-0x241f, 0x2421).  This is a bit nicer
 * than embedding control characters in filenames.
 */
static const uint16_t gMORToUnicode[256] = {
    /*0x00*/  0x2400,   // [control] NULL
    /*0x01*/  0x2401,   // [control] START OF HEADING
    /*0x02*/  0x2402,   // [control] START OF TEXT
    /*0x03*/  0x2403,   // [control] END OF TEXT
    /*0x04*/  0x2404,   // [control] END OF TRANSMISSION
    /*0x05*/  0x2405,   // [control] ENQUIRY
    /*0x06*/  0x2406,   // [control] ACKNOWLEDGE
    /*0x07*/  0x2407,   // [control] BELL
    /*0x08*/  0x2408,   // [control] BACKSPACE
    /*0x09*/  0x2409,   // [control] HORIZONTAL TABULATION
    /*0x0a*/  0x240a,   // [control] LINE FEED
    /*0x0b*/  0x240b,   // [control] VERTICAL TABULATION
    /*0x0c*/  0x240c,   // [control] FORM FEED
    /*0x0d*/  0x240d,   // [control] CARRIAGE RETURN
    /*0x0e*/  0x240e,   // [control] SHIFT OUT
    /*0x0f*/  0x240f,   // [control] SHIFT IN
    /*0x10*/  0x2410,   // [control] DATA LINK ESCAPE
    /*0x11*/  0x2411,   // [control] DEVICE CONTROL ONE
    /*0x12*/  0x2412,   // [control] DEVICE CONTROL TWO
    /*0x13*/  0x2413,   // [control] DEVICE CONTROL THREE
    /*0x14*/  0x2414,   // [control] DEVICE CONTROL FOUR
    /*0x15*/  0x2415,   // [control] NEGATIVE ACKNOWLEDGE
    /*0x16*/  0x2416,   // [control] SYNCHRONOUS IDLE
    /*0x17*/  0x2417,   // [control] END OF TRANSMISSION BLOCK
    /*0x18*/  0x2418,   // [control] CANCEL
    /*0x19*/  0x2419,   // [control] END OF MEDIUM
    /*0x1a*/  0x241a,   // [control] SUBSTITUTE
    /*0x1b*/  0x241b,   // [control] ESCAPE
    /*0x1c*/  0x241c,   // [control] FILE SEPARATOR
    /*0x1d*/  0x241d,   // [control] GROUP SEPARATOR
    /*0x1e*/  0x241e,   // [control] RECORD SEPARATOR
    /*0x1f*/  0x241f,   // [control] UNIT SEPARATOR
    /*0x20*/  0x0020,   // SPACE
    /*0x21*/  0x0021,   // EXCLAMATION MARK
    /*0x22*/  0x0022,   // QUOTATION MARK
    /*0x23*/  0x0023,   // NUMBER SIGN
    /*0x24*/  0x0024,   // DOLLAR SIGN
    /*0x25*/  0x0025,   // PERCENT SIGN
    /*0x26*/  0x0026,   // AMPERSAND
    /*0x27*/  0x0027,   // APOSTROPHE
    /*0x28*/  0x0028,   // LEFT PARENTHESIS
    /*0x29*/  0x0029,   // RIGHT PARENTHESIS
    /*0x2A*/  0x002A,   // ASTERISK
    /*0x2B*/  0x002B,   // PLUS SIGN
    /*0x2C*/  0x002C,   // COMMA
    /*0x2D*/  0x002D,   // HYPHEN-MINUS
    /*0x2E*/  0x002E,   // FULL STOP
    /*0x2F*/  0x002F,   // SOLIDUS
    /*0x30*/  0x0030,   // DIGIT ZERO
    /*0x31*/  0x0031,   // DIGIT ONE
    /*0x32*/  0x0032,   // DIGIT TWO
    /*0x33*/  0x0033,   // DIGIT THREE
    /*0x34*/  0x0034,   // DIGIT FOUR
    /*0x35*/  0x0035,   // DIGIT FIVE
    /*0x36*/  0x0036,   // DIGIT SIX
    /*0x37*/  0x0037,   // DIGIT SEVEN
    /*0x38*/  0x0038,   // DIGIT EIGHT
    /*0x39*/  0x0039,   // DIGIT NINE
    /*0x3A*/  0x003A,   // COLON
    /*0x3B*/  0x003B,   // SEMICOLON
    /*0x3C*/  0x003C,   // LESS-THAN SIGN
    /*0x3D*/  0x003D,   // EQUALS SIGN
    /*0x3E*/  0x003E,   // GREATER-THAN SIGN
    /*0x3F*/  0x003F,   // QUESTION MARK
    /*0x40*/  0x0040,   // COMMERCIAL AT
    /*0x41*/  0x0041,   // LATIN CAPITAL LETTER A
    /*0x42*/  0x0042,   // LATIN CAPITAL LETTER B
    /*0x43*/  0x0043,   // LATIN CAPITAL LETTER C
    /*0x44*/  0x0044,   // LATIN CAPITAL LETTER D
    /*0x45*/  0x0045,   // LATIN CAPITAL LETTER E
    /*0x46*/  0x0046,   // LATIN CAPITAL LETTER F
    /*0x47*/  0x0047,   // LATIN CAPITAL LETTER G
    /*0x48*/  0x0048,   // LATIN CAPITAL LETTER H
    /*0x49*/  0x0049,   // LATIN CAPITAL LETTER I
    /*0x4A*/  0x004A,   // LATIN CAPITAL LETTER J
    /*0x4B*/  0x004B,   // LATIN CAPITAL LETTER K
    /*0x4C*/  0x004C,   // LATIN CAPITAL LETTER L
    /*0x4D*/  0x004D,   // LATIN CAPITAL LETTER M
    /*0x4E*/  0x004E,   // LATIN CAPITAL LETTER N
    /*0x4F*/  0x004F,   // LATIN CAPITAL LETTER O
    /*0x50*/  0x0050,   // LATIN CAPITAL LETTER P
    /*0x51*/  0x0051,   // LATIN CAPITAL LETTER Q
    /*0x52*/  0x0052,   // LATIN CAPITAL LETTER R
    /*0x53*/  0x0053,   // LATIN CAPITAL LETTER S
    /*0x54*/  0x0054,   // LATIN CAPITAL LETTER T
    /*0x55*/  0x0055,   // LATIN CAPITAL LETTER U
    /*0x56*/  0x0056,   // LATIN CAPITAL LETTER V
    /*0x57*/  0x0057,   // LATIN CAPITAL LETTER W
    /*0x58*/  0x0058,   // LATIN CAPITAL LETTER X
    /*0x59*/  0x0059,   // LATIN CAPITAL LETTER Y
    /*0x5A*/  0x005A,   // LATIN CAPITAL LETTER Z
    /*0x5B*/  0x005B,   // LEFT SQUARE BRACKET
    /*0x5C*/  0x005C,   // REVERSE SOLIDUS
    /*0x5D*/  0x005D,   // RIGHT SQUARE BRACKET
    /*0x5E*/  0x005E,   // CIRCUMFLEX ACCENT
    /*0x5F*/  0x005F,   // LOW LINE
    /*0x60*/  0x0060,   // GRAVE ACCENT
    /*0x61*/  0x0061,   // LATIN SMALL LETTER A
    /*0x62*/  0x0062,   // LATIN SMALL LETTER B
    /*0x63*/  0x0063,   // LATIN SMALL LETTER C
    /*0x64*/  0x0064,   // LATIN SMALL LETTER D
    /*0x65*/  0x0065,   // LATIN SMALL LETTER E
    /*0x66*/  0x0066,   // LATIN SMALL LETTER F
    /*0x67*/  0x0067,   // LATIN SMALL LETTER G
    /*0x68*/  0x0068,   // LATIN SMALL LETTER H
    /*0x69*/  0x0069,   // LATIN SMALL LETTER I
    /*0x6A*/  0x006A,   // LATIN SMALL LETTER J
    /*0x6B*/  0x006B,   // LATIN SMALL LETTER K
    /*0x6C*/  0x006C,   // LATIN SMALL LETTER L
    /*0x6D*/  0x006D,   // LATIN SMALL LETTER M
    /*0x6E*/  0x006E,   // LATIN SMALL LETTER N
    /*0x6F*/  0x006F,   // LATIN SMALL LETTER O
    /*0x70*/  0x0070,   // LATIN SMALL LETTER P
    /*0x71*/  0x0071,   // LATIN SMALL LETTER Q
    /*0x72*/  0x0072,   // LATIN SMALL LETTER R
    /*0x73*/  0x0073,   // LATIN SMALL LETTER S
    /*0x74*/  0x0074,   // LATIN SMALL LETTER T
    /*0x75*/  0x0075,   // LATIN SMALL LETTER U
    /*0x76*/  0x0076,   // LATIN SMALL LETTER V
    /*0x77*/  0x0077,   // LATIN SMALL LETTER W
    /*0x78*/  0x0078,   // LATIN SMALL LETTER X
    /*0x79*/  0x0079,   // LATIN SMALL LETTER Y
    /*0x7A*/  0x007A,   // LATIN SMALL LETTER Z
    /*0x7B*/  0x007B,   // LEFT CURLY BRACKET
    /*0x7C*/  0x007C,   // VERTICAL LINE
    /*0x7D*/  0x007D,   // RIGHT CURLY BRACKET
    /*0x7E*/  0x007E,   // TILDE
    /*0x7f*/  0x2421,   // [control] DELETE
    /*0x80*/  0x00C4,   // LATIN CAPITAL LETTER A WITH DIAERESIS
    /*0x81*/  0x00C5,   // LATIN CAPITAL LETTER A WITH RING ABOVE
    /*0x82*/  0x00C7,   // LATIN CAPITAL LETTER C WITH CEDILLA
    /*0x83*/  0x00C9,   // LATIN CAPITAL LETTER E WITH ACUTE
    /*0x84*/  0x00D1,   // LATIN CAPITAL LETTER N WITH TILDE
    /*0x85*/  0x00D6,   // LATIN CAPITAL LETTER O WITH DIAERESIS
    /*0x86*/  0x00DC,   // LATIN CAPITAL LETTER U WITH DIAERESIS
    /*0x87*/  0x00E1,   // LATIN SMALL LETTER A WITH ACUTE
    /*0x88*/  0x00E0,   // LATIN SMALL LETTER A WITH GRAVE
    /*0x89*/  0x00E2,   // LATIN SMALL LETTER A WITH CIRCUMFLEX
    /*0x8A*/  0x00E4,   // LATIN SMALL LETTER A WITH DIAERESIS
    /*0x8B*/  0x00E3,   // LATIN SMALL LETTER A WITH TILDE
    /*0x8C*/  0x00E5,   // LATIN SMALL LETTER A WITH RING ABOVE
    /*0x8D*/  0x00E7,   // LATIN SMALL LETTER C WITH CEDILLA
    /*0x8E*/  0x00E9,   // LATIN SMALL LETTER E WITH ACUTE
    /*0x8F*/  0x00E8,   // LATIN SMALL LETTER E WITH GRAVE
    /*0x90*/  0x00EA,   // LATIN SMALL LETTER E WITH CIRCUMFLEX
    /*0x91*/  0x00EB,   // LATIN SMALL LETTER E WITH DIAERESIS
    /*0x92*/  0x00ED,   // LATIN SMALL LETTER I WITH ACUTE
    /*0x93*/  0x00EC,   // LATIN SMALL LETTER I WITH GRAVE
    /*0x94*/  0x00EE,   // LATIN SMALL LETTER I WITH CIRCUMFLEX
    /*0x95*/  0x00EF,   // LATIN SMALL LETTER I WITH DIAERESIS
    /*0x96*/  0x00F1,   // LATIN SMALL LETTER N WITH TILDE
    /*0x97*/  0x00F3,   // LATIN SMALL LETTER O WITH ACUTE
    /*0x98*/  0x00F2,   // LATIN SMALL LETTER O WITH GRAVE
    /*0x99*/  0x00F4,   // LATIN SMALL LETTER O WITH CIRCUMFLEX
    /*0x9A*/  0x00F6,   // LATIN SMALL LETTER O WITH DIAERESIS
    /*0x9B*/  0x00F5,   // LATIN SMALL LETTER O WITH TILDE
    /*0x9C*/  0x00FA,   // LATIN SMALL LETTER U WITH ACUTE
    /*0x9D*/  0x00F9,   // LATIN SMALL LETTER U WITH GRAVE
    /*0x9E*/  0x00FB,   // LATIN SMALL LETTER U WITH CIRCUMFLEX
    /*0x9F*/  0x00FC,   // LATIN SMALL LETTER U WITH DIAERESIS
    /*0xA0*/  0x2020,   // DAGGER
    /*0xA1*/  0x00B0,   // DEGREE SIGN
    /*0xA2*/  0x00A2,   // CENT SIGN
    /*0xA3*/  0x00A3,   // POUND SIGN
    /*0xA4*/  0x00A7,   // SECTION SIGN
    /*0xA5*/  0x2022,   // BULLET
    /*0xA6*/  0x00B6,   // PILCROW SIGN
    /*0xA7*/  0x00DF,   // LATIN SMALL LETTER SHARP S
    /*0xA8*/  0x00AE,   // REGISTERED SIGN
    /*0xA9*/  0x00A9,   // COPYRIGHT SIGN
    /*0xAA*/  0x2122,   // TRADE MARK SIGN
    /*0xAB*/  0x00B4,   // ACUTE ACCENT
    /*0xAC*/  0x00A8,   // DIAERESIS
    /*0xAD*/  0x2260,   // NOT EQUAL TO
    /*0xAE*/  0x00C6,   // LATIN CAPITAL LETTER AE
    /*0xAF*/  0x00D8,   // LATIN CAPITAL LETTER O WITH STROKE
    /*0xB0*/  0x221E,   // INFINITY
    /*0xB1*/  0x00B1,   // PLUS-MINUS SIGN
    /*0xB2*/  0x2264,   // LESS-THAN OR EQUAL TO
    /*0xB3*/  0x2265,   // GREATER-THAN OR EQUAL TO
    /*0xB4*/  0x00A5,   // YEN SIGN
    /*0xB5*/  0x00B5,   // MICRO SIGN
    /*0xB6*/  0x2202,   // PARTIAL DIFFERENTIAL
    /*0xB7*/  0x2211,   // N-ARY SUMMATION
    /*0xB8*/  0x220F,   // N-ARY PRODUCT
    /*0xB9*/  0x03C0,   // GREEK SMALL LETTER PI
    /*0xBA*/  0x222B,   // INTEGRAL
    /*0xBB*/  0x00AA,   // FEMININE ORDINAL INDICATOR
    /*0xBC*/  0x00BA,   // MASCULINE ORDINAL INDICATOR
    /*0xBD*/  0x03A9,   // GREEK CAPITAL LETTER OMEGA
    /*0xBE*/  0x00E6,   // LATIN SMALL LETTER AE
    /*0xBF*/  0x00F8,   // LATIN SMALL LETTER O WITH STROKE
    /*0xC0*/  0x00BF,   // INVERTED QUESTION MARK
    /*0xC1*/  0x00A1,   // INVERTED EXCLAMATION MARK
    /*0xC2*/  0x00AC,   // NOT SIGN
    /*0xC3*/  0x221A,   // SQUARE ROOT
    /*0xC4*/  0x0192,   // LATIN SMALL LETTER F WITH HOOK
    /*0xC5*/  0x2248,   // ALMOST EQUAL TO
    /*0xC6*/  0x2206,   // INCREMENT
    /*0xC7*/  0x00AB,   // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    /*0xC8*/  0x00BB,   // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    /*0xC9*/  0x2026,   // HORIZONTAL ELLIPSIS
    /*0xCA*/  0x00A0,   // NO-BREAK SPACE
    /*0xCB*/  0x00C0,   // LATIN CAPITAL LETTER A WITH GRAVE
    /*0xCC*/  0x00C3,   // LATIN CAPITAL LETTER A WITH TILDE
    /*0xCD*/  0x00D5,   // LATIN CAPITAL LETTER O WITH TILDE
    /*0xCE*/  0x0152,   // LATIN CAPITAL LIGATURE OE
    /*0xCF*/  0x0153,   // LATIN SMALL LIGATURE OE
    /*0xD0*/  0x2013,   // EN DASH
    /*0xD1*/  0x2014,   // EM DASH
    /*0xD2*/  0x201C,   // LEFT DOUBLE QUOTATION MARK
    /*0xD3*/  0x201D,   // RIGHT DOUBLE QUOTATION MARK
    /*0xD4*/  0x2018,   // LEFT SINGLE QUOTATION MARK
    /*0xD5*/  0x2019,   // RIGHT SINGLE QUOTATION MARK
    /*0xD6*/  0x00F7,   // DIVISION SIGN
    /*0xD7*/  0x25CA,   // LOZENGE
    /*0xD8*/  0x00FF,   // LATIN SMALL LETTER Y WITH DIAERESIS
    /*0xD9*/  0x0178,   // LATIN CAPITAL LETTER Y WITH DIAERESIS
    /*0xDA*/  0x2044,   // FRACTION SLASH
    /*0xDB*/  0x00A4,   // CURRENCY SIGN (was EURO SIGN)
    /*0xDC*/  0x2039,   // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    /*0xDD*/  0x203A,   // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    /*0xDE*/  0xFB01,   // LATIN SMALL LIGATURE FI
    /*0xDF*/  0xFB02,   // LATIN SMALL LIGATURE FL
    /*0xE0*/  0x2021,   // DOUBLE DAGGER
    /*0xE1*/  0x00B7,   // MIDDLE DOT
    /*0xE2*/  0x201A,   // SINGLE LOW-9 QUOTATION MARK
    /*0xE3*/  0x201E,   // DOUBLE LOW-9 QUOTATION MARK
    /*0xE4*/  0x2030,   // PER MILLE SIGN
    /*0xE5*/  0x00C2,   // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    /*0xE6*/  0x00CA,   // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
    /*0xE7*/  0x00C1,   // LATIN CAPITAL LETTER A WITH ACUTE
    /*0xE8*/  0x00CB,   // LATIN CAPITAL LETTER E WITH DIAERESIS
    /*0xE9*/  0x00C8,   // LATIN CAPITAL LETTER E WITH GRAVE
    /*0xEA*/  0x00CD,   // LATIN CAPITAL LETTER I WITH ACUTE
    /*0xEB*/  0x00CE,   // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
    /*0xEC*/  0x00CF,   // LATIN CAPITAL LETTER I WITH DIAERESIS
    /*0xED*/  0x00CC,   // LATIN CAPITAL LETTER I WITH GRAVE
    /*0xEE*/  0x00D3,   // LATIN CAPITAL LETTER O WITH ACUTE
    /*0xEF*/  0x00D4,   // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
    /*0xF0*/  0xF8FF,   // Apple logo
    /*0xF1*/  0x00D2,   // LATIN CAPITAL LETTER O WITH GRAVE
    /*0xF2*/  0x00DA,   // LATIN CAPITAL LETTER U WITH ACUTE
    /*0xF3*/  0x00DB,   // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
    /*0xF4*/  0x00D9,   // LATIN CAPITAL LETTER U WITH GRAVE
    /*0xF5*/  0x0131,   // LATIN SMALL LETTER DOTLESS I
    /*0xF6*/  0x02C6,   // MODIFIER LETTER CIRCUMFLEX ACCENT
    /*0xF7*/  0x02DC,   // SMALL TILDE
    /*0xF8*/  0x00AF,   // MACRON
    /*0xF9*/  0x02D8,   // BREVE
    /*0xFA*/  0x02D9,   // DOT ABOVE
    /*0xFB*/  0x02DA,   // RING ABOVE
    /*0xFC*/  0x00B8,   // CEDILLA
    /*0xFD*/  0x02DD,   // DOUBLE ACUTE ACCENT
    /*0xFE*/  0x02DB,   // OGONEK
    /*0xFF*/  0x02C7    // CARON
};

/*
 * Static table, populated on first use.  Provides the inverse map.
 *
 * An entry with 0x00 indicates no conversion.  That's incorrect for
 * the entry for '\0', but since we're operating on null-terminated
 * strings that's never valid anyway.  (It's possible for a filename
 * to contain 0x2400, but that would translate to 0x00, which we don't
 * allow; so it makes more sense to treat it as illegal.)
 */
static uint8_t gUnicodeToMOR[65536] = { 0xff /*indicates not initialized*/ };

static void Nu_GenerateUnicodeToMOR(void)
{
    memset(gUnicodeToMOR, 0, sizeof(gUnicodeToMOR));

    int i;
    for (i = 0; i < 256; i++) {
        int codePoint = gMORToUnicode[i];
        Assert(codePoint >= 0 && codePoint < 65536);
        gUnicodeToMOR[codePoint] = i;
    }
}


/*
 * Converts stringMOR to Unicode, storing the output in bufUNI until it's
 * full.  Null termination is guaranteed.  If the buffer size is zero or
 * bufUNI is NULL, no string data is returned.
 *
 * Returns the number of bytes required to represent stringMOR in Unicode.
 */
size_t Nu_ConvertMORToUNI(const char* stringMOR, UNICHAR* bufUNI,
    size_t bufSize)
{
    Assert(stringMOR != 0);

#ifdef _WIN32
    /* place-holder if we're not using UTF-16 yet */
    Assert(sizeof(UNICHAR) == 1);
    size_t morLen = strlen(stringMOR) + 1;
    if (bufUNI != NULL && bufSize != 0) {
        size_t copyLen = morLen < bufSize ? morLen : bufSize;
        memcpy(bufUNI, stringMOR, copyLen);
        bufUNI[bufSize-1] = '\0';
    }
    return morLen;
#else
    /*
     * Convert Mac OS Roman to UTF-8.  We only output full code points,
     * so if only the first byte of a UTF-8 sequence will fit we just
     * stop early.
     */
    size_t uniLen = 0;
    Boolean doOutput = (bufUNI != NULL);

    while (*stringMOR != '\0') {
        // ASCII values just "convert" to themselves in this table
        uint16_t us = gMORToUnicode[(uint8_t)*stringMOR];
        if (us < 0x80) {
            // single byte, no conversion
            if (uniLen+1 >= bufSize) {
                doOutput = false;
            }
            if (doOutput) {
                bufUNI[uniLen] = (char) us;
            }
            uniLen++;
        } else if (us < 0x7ff) {
            // two bytes
            if (uniLen+2 >= bufSize) {
                doOutput = false;
            }
            if (doOutput) {
                bufUNI[uniLen] = (us >> 6) | 0xc0;
                bufUNI[uniLen+1] = (us & 0x3f) | 0x80;
            }
            uniLen += 2;
        } else {
            // three bytes
            if (uniLen+3 >= bufSize) {
                doOutput = false;
            }
            if (doOutput) {
                bufUNI[uniLen] = (us >> 12) | 0xe0;
                bufUNI[uniLen+1] = ((us >> 6) & 0x3f) | 0x80;
                bufUNI[uniLen+2] = (us & 0x3f) | 0x80;
            }
            uniLen += 3;
        }

        stringMOR++;
    }

    // null-terminate
    if (doOutput && uniLen < bufSize) {
        bufUNI[uniLen] = '\0';
    }
    uniLen++;

    return uniLen;
#endif
}

/*
 * Decode a single Unicode code point from a UTF-8 string.  This will
 * consume 1 to 4 bytes.  If an error is detected, only one byte is
 * consumed, and the code point value will be 0xDCnn (invalid).
 *
 * cf. http://en.wikipedia.org/wiki/UTF-8#Sample_code
 */
static uint32_t Nu_DecodeUTF8(const char** pStr)
{
    const uint8_t* str = (const uint8_t*) *pStr;
    uint32_t codePoint;
    uint32_t uc1, uc2, uc3, uc4;
    uc1 = *str++;

    if (uc1 < 0x80) {
        // single byte
        codePoint = uc1;
    } else if (uc1 < 0xc2) {
        // illegal: continuation or overlong 2-byte sequence
        goto fail;
    } else if (uc1 < 0xe0) {
        // 2-byte sequence
        uc2 = *str++;
        if ((uc2 & 0xc0) != 0x80) {
            goto fail;  // not a continuation
        }
        codePoint = (uc1 << 6) + uc2 - 0x3080;
    } else if (uc1 < 0xf0) {
        // 3-byte sequence */
        uc2 = *str++;
        if ((uc2 & 0xc0) != 0x80) {
            goto fail;  // not a continuation
        }
        if (uc1 == 0xe0 && uc2 < 0xa0) {
            goto fail;  // overlong
        }
        uc3 = *str++;
        if ((uc3 & 0xc0) != 0x80) {
            goto fail;  // not a continuation
        }
        codePoint = (uc1 << 12) + (uc2 << 6) + uc3 - 0xE2080;
    } else if (uc1 < 0xf5) {
        uc2 = *str++;
        if ((uc2 & 0xc0) != 0x80) {
            goto fail;  // not a continuation
        }
        if (uc1 == 0xf0 && uc2 < 0x90) {
            goto fail;  // overlong
        }
        if (uc1 == 0xf4 && uc2 >= 0x90) {
            goto fail;  // U+10FFFF 
        }
        uc3 = *str++;
        if ((uc3 & 0xc0) != 0x80) {
            goto fail;  // not a continuation
        }
        uc4 = *str++;
        if ((uc4 & 0xc0) != 0x80) {
            goto fail;  // not a continuation
        }
        codePoint = (uc1 << 18) + (uc2 << 12) + (uc3 << 6) + uc4 - 0x3C82080;
    } else {
        // illegal: > U+10FFFF
        goto fail;
    }

    *pStr = (const UNICHAR*) str;
    return codePoint;

fail:
    (*pStr)++;              // advance one char only
    return 0xdc00 | uc1;
}

/*
 * Converts stringUNI to Mac OS Roman, storing the output in bufMOR
 * until it's full.  Null termination is guaranteed.  If the buffer
 * size is zero or bufMOR is NULL, no string data is returned.
 *
 * Returns the number of bytes required to represent stringUNI in MOR.
 */
size_t Nu_ConvertUNIToMOR(const UNICHAR* stringUNI, char* bufMOR,
    size_t bufSize)
{
    Assert(stringUNI != 0);

#ifdef _WIN32
    /*
     * Place-holder if we're not using UTF-16 yet.  This doesn't pass
     * tests that check for behavior with non-MOR Unicode values.
     */
    Assert(sizeof(UNICHAR) == 1);
    size_t uniLen = strlen(stringUNI) + 1;
    if (bufMOR != NULL && bufSize != 0) {
        size_t copyLen = uniLen < bufSize ? uniLen : bufSize;
        memcpy(bufMOR, stringUNI, copyLen);
        bufMOR[bufSize-1] = '\0';
    }
    return uniLen;
#else
    /*
     * Convert UTF-8 to Mac OS Roman.  If the code point doesn't have
     * a valid conversion (either because it's not in the table, or the
     * UTF-8 code is damaged) we just insert an ASCII '?'.
     */
    if (gUnicodeToMOR[0] == 0xff) {
        Nu_GenerateUnicodeToMOR();
        Assert(gUnicodeToMOR[0] != 0xff);
    }

    uint32_t codePoint;
    size_t morLen = 0;
    Boolean doOutput = (bufMOR != NULL);

    while (*stringUNI != '\0') {
        codePoint = Nu_DecodeUTF8(&stringUNI);
        char mc;

        if (codePoint < 0x80) {
            mc = (char) codePoint;
        } else if (codePoint < 0xffff) {
            // UTF-8 errors come back as 0xDCnn, which has no mapping in table
            mc = gUnicodeToMOR[codePoint];
            if (mc == 0x00) {
                mc = '?';
            }
        } else {
            // non-BMP code point
            mc = '?';
        }
        if (morLen+1 >= bufSize) {
            doOutput = false;
        }
        if (doOutput) {
            bufMOR[morLen] = mc;
        }
        morLen++;
    }

    // null-terminate
    if (doOutput && morLen < bufSize) {
        bufMOR[morLen] = '\0';
    }
    morLen++;

    return morLen;
#endif
}

/*
 * Utility function that wraps NuConvertMORToUTF8, allocating a new
 * buffer to hold the converted string.  The caller must free the result.
 *
 * Returns NULL if stringMOR is NULL or the conversion fails.
 */
UNICHAR* Nu_CopyMORToUNI(const char* stringMOR)
{
    size_t uniLen;
    UNICHAR* uniBuf;

    if (stringMOR == NULL) {
        return NULL;
    }

    uniLen = Nu_ConvertMORToUNI(stringMOR, NULL, 0);
    if (uniLen == (size_t) -1) {
        return NULL;
    }
    uniBuf = (UNICHAR*) Nu_Malloc(NULL, uniLen);
    Nu_ConvertMORToUNI(stringMOR, uniBuf, uniLen);
    return uniBuf;
}
