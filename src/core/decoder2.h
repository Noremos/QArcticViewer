
#include "../base.h"

#include <QDebug>

#include <chrono>
#include <vector>
#include <array>
#include <iomanip>
#include <algorithm>



//make shure result have enough size
bool decompressLZW(uchar* input, offu64 size, buffer& result, int bytesPerRow)
/*
    Works for RGB but not for RRGGBB (planarConfiguration = 2).  Requires tweak to pBuf.
*/
{
//    result.reserve(bytesPerRow);
    const unsigned int CLEAR_CODE = 256;
    const unsigned int EOF_CODE = 257;
    const unsigned int MAXCODE = 4095;      // 12 bit max less some head room

    #define LZW_STRING_SIZE 256
    #define LZW_STRINGS_SIZE 128000

    bool ret = false;
    // input and output pointers
    char* c = reinterpret_cast<char*>(input);
    char* out = reinterpret_cast<char*>(result.data());

    char* s[4096];                                  // ptrs in strings for each possible code
    int8_t sLen[4096];                              // code string length
    memset(&sLen, 1, 256);                     // 0-255 one char strings

    char strings[LZW_STRINGS_SIZE];
    // initialize first 256 code strings
    for (int i = 0 ; i != 256 ; i++ ) {
        strings[i] = (char)i;
        s[i] = &strings[i];
    }
    strings[256] = 0;  s[256] = &strings[256];      // Clear code
    strings[257] = 0;  s[257] = &strings[257];      // EOF code
    const uint32_t maxCode = 4095;                  // max for 12 bits
    char* sEnd;                                     // ptr to current end of strings

    char ps[LZW_STRING_SIZE];                       // previous string
    size_t psLen = 0;                               // length of prevString
    uint32_t code;                                  // key to string for code
    uint32_t nextCode = 258;                        // used to preset string for next
    uint32_t incoming = (uint32_t)size;      // count down input chars
    int n = 0;                                      // output byte counter
    uint32_t iBuf = 0;                              // incoming bit buffer
    int32_t nBits = 0;                              // incoming bits in the buffer
    int32_t codeBits = 9;                           // number of bits to make code (9-12)
    uint32_t nextBump = 511;                        // when to increment code size 1st time
    uint32_t pBuf = 0;                              // previous out bit buffer
    uint32_t mask = (1 << codeBits) - 1;            // extract code from iBuf

    uint32_t* pSrc;                                 // ptr to src for word copies
    uint32_t* pDst;                                 // ptr to dst for word copies

    // read incoming bytes into the bit buffer (iBuf) using the char pointer c
    while (incoming)
    {
        // GetNextCode
        iBuf = (iBuf << 8) | (uint8_t)*c++;         // make room in bit buf for char
        nBits += 8;
        --incoming;
        if (nBits < codeBits) {
            iBuf = (iBuf << 8) | (uint8_t)*c++;     // make room in bit buf for char
            nBits += 8;
            --incoming;
        }
        code = (iBuf >> (nBits - codeBits)) & mask; // extract code from buffer
        nBits -= codeBits;                          // update available bits to process

        // rest at start and when codes = max ~+ 4094
        if (code == CLEAR_CODE) {
            codeBits = 9;
            mask = (1 << codeBits) - 1;
            nextBump = 511;
            sEnd = s[257];
            nextCode = 258;
            psLen = 0;
            continue;
        }

        // finished (should not need as incoming counts down to zero)
        if (code == EOF_CODE)
        {
            return ret;
        }

        // new code then add prevString + prevString[0]
        // copy prevString
        if (code == nextCode)
        {
            s[code] = sEnd;
            switch(psLen) {
            case 1:
                *s[code] = ps[0];
                break;
            case 2:
                *s[code] = ps[0];
                *(s[code]+1) = ps[1];
                break;
            case 4:
                pDst = (uint32_t*)s[code];
                pSrc = (uint32_t*)&ps;
                *pDst = *pSrc;
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                pDst = (uint32_t*)s[nextCode];
                pSrc = (uint32_t*)&ps;
                *pDst = *pSrc;
                *(pDst+1) = *(pSrc+1);
                break;
            default:
                memcpy(s[code], &ps, psLen);
            }

            // copy prevString[0]
            *(s[code] + psLen) = ps[0];
            sLen[code] = (int8_t)psLen + 1;
            sEnd = s[code] + psLen + 1;
        }

//        if (predictor)
        {
            for (uint32_t i = 0; i != (uint32_t)sLen[code]; i++)
            {
                if (n % bytesPerRow < 3)
                {
                    *out++ = *(s[code] + i);
                }
                else
                {
                    *out++ = *(s[code] + i) + *(out - 3);
                }
                ++n;
                /*
                // output char string for code (add from left)
                // pBuf   00000000 11111111 22222222 33333333
                if (n % bytesPerRow == 0) pBuf = 0;
                char b = *(s[code] + i) + (uint8_t)(pBuf & 0xFF);
                *out++ = b;
                pBuf = (pBuf >> 8) | (uint32_t)((uint8_t)b << 16);
                ++n;
                */
            }
        }
//        else {
//            for (uint32_t i = 0; i != (uint32_t)sLen[code]; i++)
//            {
//                uchar b = (uchar)*(s[code] + i);
//                *out++ = *(s[code] + i);
//                if (n == 26400) {
//                    int xxx = 0;
//                }
//                ++n;
//            }
//        }

        // add string to nextCode (prevString + strings[code][0])
        // copy prevString
        if (psLen/* && nextCode <= MAXCODE*/)
        {
            s[nextCode] = sEnd;
            switch(psLen) {
            case 1:
                *s[nextCode] = ps[0];
                break;
            case 2:
                *s[nextCode] = ps[0];
                *(s[nextCode]+1) = ps[1];
                break;
            case 4:
                pDst = (uint32_t*)s[nextCode];
                pSrc = (uint32_t*)&ps;
                *pDst = *pSrc;
                break;
            case 5:
            case 6:
            case 7:
            case 8:
                pDst = (uint32_t*)s[nextCode];
                pSrc = (uint32_t*)&ps;
                *pDst = *pSrc;
                *(pDst+1) = *(pSrc+1);
                break;
            default:
                memcpy(s[nextCode], &ps, psLen);
            }

            // copy strings[code][0]
            *(s[nextCode] + psLen) = *s[code];

            sLen[nextCode] = (int8_t)(psLen + 1);
            sEnd = s[nextCode] + psLen + 1;
            ++nextCode;
        }

        // strings[code][0] copy
        switch(sLen[code]) {
        case 1:
            ps[0] = *s[code];
            break;
        case 2:
            ps[0] = *s[code];
            ps[1] = *(s[code]+1);
            break;
        case 4:
            pSrc = (uint32_t*)s[code];
            pDst = (uint32_t*)&ps;
            *pDst = *pSrc;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            pSrc = (uint32_t*)s[code];
            pDst = (uint32_t*)&ps;
            *pDst = *pSrc;
            *(pDst+1) = *(pSrc+1);
            break;
        default:
            memcpy(&ps, s[code], (size_t)sLen[code]);
        }

        psLen = (size_t)sLen[code];

         // codeBits change
        if (nextCode == nextBump) {
            if (nextCode < maxCode) {
                nextBump = (nextBump << 1) + 1;
                ++codeBits;
                mask = (1 << codeBits) - 1;
            }
            else if (nextCode == maxCode) continue;
            else {
                codeBits = 9;
                mask = (1 << codeBits) - 1;
                nextBump = 511;
                sEnd = s[257];
                nextCode = 258;
                psLen = 0;
            }
        }

    } // end while}

    return ret;
}
