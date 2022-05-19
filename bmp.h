#pragma once

#include<stdio.h>
typedef unsigned int DWORD;  // 4bytes
typedef unsigned short WORD;  // 2bytes
typedef signed long LONG;  // 4bytes
typedef unsigned char BYTE;  // 1bytes


#pragma pack(push)
#pragma pack(1)// 修改默认对齐值
/*位图文件文件头结构体*/
typedef struct tagBITMAPFILEHEADER {
	WORD bFileType;
	DWORD bFileSize;
	WORD bReserved1;
	WORD bReserved2;
	DWORD bPixelDataOffset;
}BITMAPFILEHEADER; //14bytes
#pragma pack(pop)

/*位图文件信息头结构体*/
typedef struct tagBITMAPINFOHEADER {
	DWORD bHeaderSize;  // 图像信息头总大小（40bytes）
	LONG bImageWidth;  // 图像宽度（像素）
	LONG bImageHeight;  // 图像高度
	WORD bPlanes;  // 应该是0
	WORD bBitsPerPixel;  // 像素位数
	DWORD bCompression;  // 图像压缩方法
	DWORD bImageSize;  // 图像大小（字节）
	LONG bXpixelsPerMeter;  // 横向每米像素数
	LONG bYpixelsPerMeter;  // 纵向每米像素数
	DWORD bTotalColors;  // 使用的颜色总数，如果像素位数大于8，则该字段没有意义
	DWORD bImportantColors;  // 重要颜色数，一般没什么用
}BITMAPINFOHEADER; //40bytes

/*位图文件调色板结构体*/
typedef struct tagRGBQUAD {
	BYTE	rgbBlue;
	BYTE	rgbGreen;
	BYTE	rgbRed;
	BYTE	rgbReserved;
}RGBQUAD;

/*像素点RGB结构体*/
typedef struct tagRGB {
	BYTE blue;
	BYTE green;
	BYTE red;
}RGBDATA;
