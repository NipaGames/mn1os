#include "utf8.h"


int is_utf8(WCHAR c) {
    uint8_t byte1 = c >> 8 & 0xff;
    uint8_t byte2 = c & 0xff;
    return (byte1 & 0xc0) == 0xc0 && (byte2 & 0xc0) == 0x80;
}

uint8_t utf8_to_cp437(WCHAR c) {
    switch (c) {
        case 0xc2a1: return 0xad; // ¡
        case 0xc2a2: return 0x9b; // ¢
        case 0xc2a3: return 0x9c; // £
        case 0xc2a5: return 0x9d; // ¥
        case 0xc2a7: return 0x15; // §
        case 0xc2aa: return 0xa6; // ª
        case 0xc2ab: return 0xae; // «
        case 0xc2ac: return 0xaa; // ¬
        case 0xc2b0: return 0xf8; // °
        case 0xc2b1: return 0xf1; // ±
        case 0xc2b2: return 0xfd; // ²
        case 0xc2b5: return 0xe6; // µ
        case 0xc2b6: return 0x14; // ¶
        case 0xc2b7: return 0xfa; // ·
        case 0xc2ba: return 0xa7; // º
        case 0xc2bb: return 0xaf; // »
        case 0xc2bc: return 0xac; // ¼
        case 0xc2bd: return 0xab; // ½
        case 0xc2bf: return 0xa8; // ¿
        case 0xc384: return 0x8e; // Ä
        case 0xc385: return 0x8f; // Å
        case 0xc386: return 0x92; // Æ
        case 0xc387: return 0x80; // Ç
        case 0xc389: return 0x90; // É
        case 0xc391: return 0xa5; // Ñ
        case 0xc396: return 0x99; // Ö
        case 0xc39c: return 0x9a; // Ü
        case 0xc39f: return 0xe1; // ß
        case 0xc3a0: return 0x85; // à
        case 0xc3a1: return 0xa0; // á
        case 0xc3a2: return 0x83; // â
        case 0xc3a4: return 0x84; // ä
        case 0xc3a5: return 0x86; // å
        case 0xc3a6: return 0x91; // æ
        case 0xc3a7: return 0x87; // ç
        case 0xc3a8: return 0x8a; // è
        case 0xc3a9: return 0x82; // é
        case 0xc3aa: return 0x88; // ê
        case 0xc3ab: return 0x89; // ë
        case 0xc3ac: return 0x8d; // ì
        case 0xc3ad: return 0xa1; // í
        case 0xc3ae: return 0x8c; // î
        case 0xc3af: return 0x8b; // ï
        case 0xc3b1: return 0xa4; // ñ
        case 0xc3b2: return 0x95; // ò
        case 0xc3b3: return 0xa2; // ó
        case 0xc3b4: return 0x93; // ô
        case 0xc3b6: return 0x94; // ö
        case 0xc3b7: return 0xf6; // ÷
        case 0xc3b9: return 0x97; // ù
        case 0xc3ba: return 0xa3; // ú
        case 0xc3bb: return 0x96; // û
        case 0xc3bc: return 0x81; // ü
        case 0xc3bf: return 0x98; // ÿ
        default: return 0xfe; // not in cp437, show an awkward box lol
    }
}

