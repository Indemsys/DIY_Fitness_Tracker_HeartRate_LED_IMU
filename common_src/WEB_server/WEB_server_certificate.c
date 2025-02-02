#include   "App.h"
#include   "WEB_server_certificate.h"

const uint8_t SERVER_KEY[121] = {
0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0xd3, 0x58, 0xcb, 0x6a, 0xdf, 0x99, 0xda, 0xf3, 0xbc, 0x27, 0x4b, 0x97, 0x84, 0xa6, 0xf3, 0x33, 0xf2, 0x86, 0x17, 0x0e, 0xb1, 0x99, 0xb6, 0xb5, 0x30,
0x2a, 0xe0, 0xb3, 0xa8, 0xed, 0xe1, 0x55, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0xa1, 0x44, 0x03, 0x42, 0x00, 0x04, 0x40, 0xa3, 0xde, 0x66, 0xed, 0xd8, 0x10,
0x64, 0xa5, 0x50, 0xd9, 0x4a, 0xdd, 0x0a, 0xf1, 0xbf, 0x1a, 0xf7, 0x42, 0x78, 0xb5, 0xc9, 0xb8, 0x33, 0x45, 0x38, 0x34, 0x5a, 0xaf, 0x6e, 0xb3, 0xc4, 0x3b, 0x59, 0x50, 0xe8, 0x4d, 0x03, 0x7a,
0x43, 0x0f, 0x31, 0x8a, 0x7d, 0x58, 0x44, 0x67, 0x64, 0x2f, 0xe6, 0xfd, 0x17, 0xe0, 0x0c, 0x66, 0x97, 0x74, 0xd9, 0x91, 0x05, 0x42, 0x23, 0xdc, 0x8c
};
const uint8_t SERVER_CERTIFICATE[569] = {
0x30, 0x82, 0x02, 0x35, 0x30, 0x82, 0x01, 0xdb, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x14, 0x16, 0x33, 0x51, 0x9e, 0xcd, 0xc6, 0xe0, 0x6a, 0x2b, 0x27, 0x79, 0x94, 0x76, 0x64, 0x35, 0x4c, 0x9d,
0x15, 0xd8, 0x05, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x30, 0x19, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x0e, 0x49, 0x6e, 0x64, 0x65,
0x6d, 0x73, 0x79, 0x73, 0x52, 0x6f, 0x6f, 0x74, 0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x34, 0x30, 0x38, 0x31, 0x32, 0x31, 0x39, 0x35, 0x30, 0x31, 0x33, 0x5a, 0x17, 0x0d, 0x33, 0x34, 0x30,
0x38, 0x31, 0x30, 0x31, 0x39, 0x35, 0x30, 0x31, 0x33, 0x5a, 0x30, 0x13, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x08, 0x69, 0x6e, 0x64, 0x65, 0x6d, 0x73, 0x79, 0x73, 0x30,
0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0x40, 0xa3, 0xde, 0x66, 0xed, 0xd8,
0x10, 0x64, 0xa5, 0x50, 0xd9, 0x4a, 0xdd, 0x0a, 0xf1, 0xbf, 0x1a, 0xf7, 0x42, 0x78, 0xb5, 0xc9, 0xb8, 0x33, 0x45, 0x38, 0x34, 0x5a, 0xaf, 0x6e, 0xb3, 0xc4, 0x3b, 0x59, 0x50, 0xe8, 0x4d, 0x03,
0x7a, 0x43, 0x0f, 0x31, 0x8a, 0x7d, 0x58, 0x44, 0x67, 0x64, 0x2f, 0xe6, 0xfd, 0x17, 0xe0, 0x0c, 0x66, 0x97, 0x74, 0xd9, 0x91, 0x05, 0x42, 0x23, 0xdc, 0x8c, 0xa3, 0x82, 0x01, 0x05, 0x30, 0x82,
0x01, 0x01, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0x32, 0x78, 0xfd, 0xaa, 0x72, 0x05, 0x49, 0x5f, 0x97, 0xd8, 0xb2, 0x73, 0x17, 0x02, 0x89, 0xc5, 0xe9, 0x0e, 0x89,
0x9c, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0xda, 0xb2, 0x10, 0x7e, 0xb7, 0x94, 0x2d, 0x6d, 0x6f, 0x91, 0x3a, 0x84, 0xbd, 0xc5, 0x4b, 0xcb, 0xd8, 0x85,
0xc7, 0x3f, 0x30, 0x09, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x05, 0xa0, 0x30, 0x79, 0x06, 0x03, 0x55, 0x1d,
0x11, 0x04, 0x72, 0x30, 0x70, 0x82, 0x05, 0x53, 0x37, 0x56, 0x33, 0x30, 0x82, 0x0b, 0x53, 0x37, 0x56, 0x33, 0x30, 0x2e, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x82, 0x08, 0x69, 0x6e, 0x64, 0x65, 0x6d,
0x73, 0x79, 0x73, 0x82, 0x0e, 0x69, 0x6e, 0x64, 0x65, 0x6d, 0x73, 0x79, 0x73, 0x2e, 0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x87, 0x04, 0xc0, 0xa8, 0x00, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x01, 0x01, 0x87,
0x04, 0xc0, 0xa8, 0x02, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x03, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x04, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x05, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x06, 0x01, 0x87, 0x04, 0xc0,
0xa8, 0x07, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x08, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x09, 0x01, 0x87, 0x04, 0xc0, 0xa8, 0x0a, 0x01, 0x30, 0x2c, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x86, 0xf8, 0x42,
0x01, 0x0d, 0x04, 0x1f, 0x16, 0x1d, 0x4f, 0x70, 0x65, 0x6e, 0x53, 0x53, 0x4c, 0x20, 0x47, 0x65, 0x6e, 0x65, 0x72, 0x61, 0x74, 0x65, 0x64, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63,
0x61, 0x74, 0x65, 0x30, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02, 0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20, 0x75, 0x33, 0x5f, 0xbc, 0x55, 0xef, 0x03, 0x13, 0x31, 0x33,
0xd0, 0x13, 0xf8, 0xe8, 0x48, 0xf9, 0x06, 0x07, 0x29, 0x9b, 0xe2, 0x55, 0x78, 0xc1, 0x75, 0x94, 0xd2, 0xc9, 0xc6, 0x82, 0x5f, 0xe9, 0x02, 0x21, 0x00, 0x95, 0x30, 0x17, 0x06, 0x3f, 0xa5, 0xfa,
0x85, 0xe3, 0xa7, 0x10, 0x00, 0x35, 0x0a, 0xa0, 0x41, 0x04, 0x7d, 0x40, 0xa6, 0xf9, 0xa4, 0xc5, 0x77, 0x80, 0x0b, 0x68, 0xd5, 0x14, 0xe3, 0xf1, 0x7c
};
