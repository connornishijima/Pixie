const uint8_t col[480] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00,  // SPACE 
	0x00, 0x00, 0x5F, 0x00, 0x00,  // !
	0x00, 0x03, 0x00, 0x03, 0x00,  // "
	0x14, 0x3E, 0x14, 0x3E, 0x14,  // #
	0x24, 0x2A, 0x7F, 0x2A, 0x12,  // $
	0x63, 0x13, 0x08, 0x64, 0x63,  // %
	0x36, 0x49, 0x59, 0x26, 0x50,  // &
	0x00, 0x00, 0x03, 0x00, 0x00,  // '
	0x00, 0x00, 0x3E, 0x41, 0x00,  // (
	0x00, 0x41, 0x3E, 0x00, 0x00,  // )
	0x00, 0x05, 0x02, 0x05, 0x00,  // *
	0x08, 0x08, 0x3E, 0x08, 0x08,  // +
	0x00, 0x58, 0x38, 0x00, 0x00,  // , 
	0x08, 0x08, 0x08, 0x08, 0x08,  // -
	0x00, 0x60, 0x60, 0x00, 0x00,  // .
	0x00, 0x60, 0x1C, 0x03, 0x00,  // /
	0x3E, 0x41, 0x49, 0x41, 0x3E,  // 0
	0x00, 0x02, 0x7F, 0x00, 0x00,  // 1
	0x46, 0x61, 0x51, 0x49, 0x46,  // 2
	0x21, 0x49, 0x4D, 0x4B, 0x31,  // 3
	0x18, 0x16, 0x10, 0x7F, 0x10,  // 4
	0x2F, 0x49, 0x49, 0x49, 0x31,  // 5
	0x3E, 0x51, 0x49, 0x49, 0x32,  // 6
	0x01, 0x01, 0x71, 0x0D, 0x03,  // 7
	0x36, 0x49, 0x49, 0x49, 0x36,  // 8
	0x26, 0x49, 0x49, 0x49, 0x3E,  // 9
	0x00, 0x36, 0x36, 0x00, 0x00,  // :
	0x00, 0x36, 0x76, 0x00, 0x00,  // ;
	0x08, 0x14, 0x14, 0x22, 0x22,  // <
	0x14, 0x14, 0x14, 0x14, 0x14,  // =
	0x22, 0x22, 0x14, 0x14, 0x08,  // >
	0x06, 0x01, 0x51, 0x09, 0x06,  // ?
	0x3E, 0x41, 0x49, 0x55, 0x1E,  // @
	0x78, 0x16, 0x11, 0x16, 0x78,  // A
	0x7F, 0x49, 0x49, 0x4E, 0x30,  // B
	0x3E, 0x41, 0x41, 0x41, 0x22,  // C
	0x41, 0x7F, 0x41, 0x41, 0x3E,  // D
	0x7F, 0x49, 0x49, 0x49, 0x41,  // E
	0x7F, 0x09, 0x09, 0x09, 0x01,  // F
	0x3E, 0x41, 0x41, 0x49, 0x7A,  // G
	0x7F, 0x08, 0x08, 0x08, 0x7F,  // H
	0x00, 0x00, 0x7F, 0x00, 0x00,  // I
	0x31, 0x41, 0x41, 0x41, 0x3F,  // J
	0x7F, 0x08, 0x08, 0x14, 0x63,  // K
	0x7F, 0x40, 0x40, 0x40, 0x40,  // L
	0x7F, 0x06, 0x08, 0x06, 0x7F,  // M
	0x7F, 0x06, 0x18, 0x60, 0x7F,  // N
	0x3E, 0x41, 0x41, 0x41, 0x3E,  // O
	0x7F, 0x09, 0x09, 0x09, 0x06,  // P
	0x3E, 0x41, 0x51, 0x21, 0x5E,  // Q
	0x7F, 0x09, 0x19, 0x29, 0x46,  // R
	0x26, 0x49, 0x49, 0x49, 0x32,  // S
	0x01, 0x01, 0x7F, 0x01, 0x01,  // T
	0x3F, 0x40, 0x40, 0x40, 0x7F,  // U
	0x0F, 0x30, 0x40, 0x30, 0x0F,  // V
	0x1F, 0x60, 0x1C, 0x60, 0x1F,  // W
	0x63, 0x14, 0x08, 0x14, 0x63,  // X
	0x03, 0x04, 0x78, 0x04, 0x03,  // Y
	0x61, 0x51, 0x49, 0x45, 0x43,  // Z
	0x00, 0x00, 0x7F, 0x41, 0x00,  // [
	0x00, 0x03, 0x1C, 0x60, 0x00,  // backslash
	0x00, 0x41, 0x7F, 0x00, 0x00,  // ]
	0x0C, 0x02, 0x01, 0x02, 0x0C,  // ^
	0x40, 0x40, 0x40, 0x40, 0x40,  // _
	0x00, 0x01, 0x02, 0x04, 0x00,  // `
	0x20, 0x54, 0x54, 0x54, 0x78,  // a
	0x7F, 0x48, 0x44, 0x44, 0x38,  // b
	0x38, 0x44, 0x44, 0x44, 0x00,  // c
	0x38, 0x44, 0x44, 0x48, 0x7F,  // d
	0x38, 0x54, 0x54, 0x54, 0x18,  // e
	0x08, 0x7E, 0x09, 0x09, 0x00,  // f
	0x0C, 0x52, 0x52, 0x54, 0x3E,  // g
	0x7F, 0x08, 0x04, 0x04, 0x78,  // h
	0x00, 0x00, 0x7A, 0x00, 0x00,  // i
	0x00, 0x40, 0x3D, 0x00, 0x00,  // j
	0x7F, 0x10, 0x28, 0x44, 0x00,  // k
	0x00, 0x00, 0x3F, 0x40, 0x00,  // l
	0x7C, 0x04, 0x78, 0x04, 0x78,  // m
	0x7C, 0x08, 0x04, 0x04, 0x78,  // n
	0x38, 0x44, 0x44, 0x44, 0x38,  // o
	0x7F, 0x12, 0x11, 0x11, 0x0E,  // p
	0x0E, 0x11, 0x11, 0x12, 0x7F,  // q
	0x7C, 0x08, 0x04, 0x04, 0x00,  // r
	0x48, 0x54, 0x54, 0x54, 0x24,  // s
	0x04, 0x3F, 0x44, 0x44, 0x00,  // t
	0x3C, 0x40, 0x40, 0x20, 0x7C,  // u
	0x1C, 0x20, 0x40, 0x20, 0x1C,  // v
	0x1C, 0x60, 0x18, 0x60, 0x1C,  // w
	0x44, 0x28, 0x10, 0x28, 0x44,  // x
	0x47, 0x28, 0x10, 0x08, 0x07,  // y
	0x44, 0x64, 0x54, 0x4C, 0x44,  // z
	0x00, 0x08, 0x77, 0x41, 0x00,  // {
	0x00, 0x00, 0x7F, 0x00, 0x00,  // |
	0x00, 0x41, 0x77, 0x08, 0x00,  // }
	0x08, 0x04, 0x0C, 0x08, 0x04   // ~
};