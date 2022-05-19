#define _CRT_SECURE_NO_DEPRECATE
#include"bmp.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include <math.h>

int IMAGE_WIDTH = 0;
int IMAGE_HEIGHT = 0;
FILE* openBmpImage(char* fileName, char* mode) {
	FILE* fp;
	if (strcmp(mode, "r") == 0) {
		mode = "rb";
	}
	else if (strcmp(mode,"w") == 0) {
		mode = "ab";
	}
	else {
		//输出错误信息
		fprintf(stderr,"文件打开模式:%s使用错误\n",mode);
		//文件打开失败，返回空指针
		return NULL;
	}
	if ((fp = fopen(fileName,mode)) == NULL) {
		fprintf(stderr, "打开文件:%s失败\n", fileName);
		return NULL;
	}
	return fp;
}

void closeBmpImage(FILE* fp) {
	//关闭文件
	fclose(fp);
	//printf("已关闭文件\n");
	//释放文件指针
	free(fp);
	//printf("已释放文件指针\n");
}

BITMAPFILEHEADER* readBmpFileHead(FILE* fp) {
	//printf("%d\n", sizeof(BITMAPFILEHEADER));//这个大小是16Bytes没错
	BITMAPFILEHEADER* fileHead = (BITMAPFILEHEADER*)malloc(sizeof(BITMAPFILEHEADER));
	if (fileHead == NULL) {
		fprintf(stderr,"内存分配失败");
	}
	if (fread(fileHead, sizeof(BITMAPFILEHEADER), 1, fp) != 1) {
		fprintf(stderr,"文件头读取失败");
	}
	return fileHead;
}

BITMAPINFOHEADER* readBmpInfoHead(FILE* fp) {
	//printf("%d\n", sizeof(BITMAPINFOHEADER));
	BITMAPINFOHEADER* infoHead = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	if (infoHead == NULL) {
		fprintf(stderr, "内存分配失败");
	}
	if (fread(infoHead, sizeof(BITMAPINFOHEADER), 1, fp) != 1) {
		fprintf(stderr, "信息头读取失败");
	}
  /*
	printf("信息头大小：%d字节\n", infoHead->bHeaderSize);
	printf("Image Width: %d pixels\n", infoHead->bImageWidth);
	printf("Image Heiht: %d pixels\n", infoHead->bImageHeight);
	printf("颜色位数：%d位\n", infoHead->bBitsPerPixel);
	printf("横向每米像素数：%d个\n", infoHead->bXpixelsPerMeter);
	printf("纵向每米像素数：%d个\n", infoHead->bYpixelsPerMeter);
	printf("数据块大小：%d字节\n", infoHead->bImageSize);
	printf("位面数：%d\n", infoHead->bPlanes);
	printf("使用颜色总数：%d个\n", infoHead->bTotalColors);
	printf("重要颜色总数：%d个\n", infoHead->bImportantColors);
	printf("压缩算法：%d\n", infoHead->bCompression);
  */

	return infoHead;
}
RGBDATA** createMatrix(int width,int height) {
	//动态创建二维数组
	RGBDATA** Matrix;
	int i;

	Matrix = (RGBDATA **)malloc(sizeof(RGBDATA*) * height);
	if (Matrix == NULL) {
		fprintf(stderr,"内存分配失败");
		return NULL;
	}

	for (i = 0; i < height; i++) {
		Matrix[i] = (RGBDATA *)malloc(sizeof(RGBDATA) * width);
		if (Matrix[i] == NULL) {
			fprintf(stderr, "内存分配失败");
			return NULL;
		}
	}
	return(Matrix);
}

RGBDATA** readBmpDataToArr(FILE* fp) {
	int i = 0, j = 0;
	int width = 0, height = 0;
	BITMAPFILEHEADER* fileHead = readBmpFileHead(fp);
	BITMAPINFOHEADER* infoHead = readBmpInfoHead(fp);
	width = infoHead->bImageWidth;
	height = infoHead->bImageHeight;

	IMAGE_WIDTH = width;
	IMAGE_HEIGHT = height;

	RGBDATA** data = createMatrix(width,height);
	//如果位数小于8则调色板有效
	if (infoHead->bBitsPerPixel < 8) {
		RGBQUAD* rgbQuad = (RGBQUAD*)malloc(sizeof(RGBQUAD));
		if(rgbQuad == NULL){
			printf("内存分配失败");
		}
		if (fread(rgbQuad, sizeof(rgbQuad), 1, fp) != 1) {
			printf("调色板读入失败");
		}
	}
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fread(&data[i][j], sizeof(RGBDATA), 1, fp);
		}
	}
	return data;
}

void print_image(RGBDATA ** data){
  for (int i = 0; i < IMAGE_HEIGHT; i++) {
		for (int j = 0; j < IMAGE_WIDTH; j++) {
			printf("Pixel coordinate at (%d, %d): [%d,%d,%d] \n ", 
			       i,j,data[i][j].red, data[i][j].green, data[i][j].blue);
		}
		printf("\n");
	}
}
void compare_image(RGBDATA ** img1, RGBDATA ** img2){
  for (int i = 0; i < IMAGE_HEIGHT; i++) {
		for (int j = 0; j < IMAGE_WIDTH; j++) {
			printf("Pixel coordinate at (%d, %d): [%d,%d,%d] \n ",
			i,j,img1[i][j].red - img2[i][j].red, img1[i][j].green - 
			       img2[i][j].green, img1[i][j].blue - img2[i][j].blue);
		}
		printf("\n");
	}
}

RGBDATA** gt_to_cfa(RGBDATA ** gt_image, int padding){
	RGBDATA ** cfa_image = createMatrix(IMAGE_WIDTH + padding * 2, IMAGE_HEIGHT+ padding * 2);
	for (int i = 0; i < IMAGE_HEIGHT + padding * 2; i++) {
		for (int j = 0; j < IMAGE_WIDTH + padding * 2; j++) {
			if ((i >= padding && i < IMAGE_HEIGHT - padding) 
			    && (j >= padding && j < IMAGE_WIDTH - padding)){
				if (i % 2 == 0 && j % 2 == 0){
					// red location
					cfa_image[i][j].red = gt_image[i-padding][j-padding].red;
					cfa_image[i][j].green = 0;
					cfa_image[i][j].blue = 0;
				}
				else if (i % 2 == 1 && j % 2 == 1){
					// blue location
					cfa_image[i][j].red = 0;
					cfa_image[i][j].green = 0;
					cfa_image[i][j].blue = gt_image[i-padding][j-padding].blue;
				}
				else {
					// green location
					cfa_image[i][j].red = 0;
					cfa_image[i][j].green = gt_image[i-padding][j-padding].green;
					cfa_image[i][j].blue = 0;
				}
			}
			else {
				// all zeros for paddings
				cfa_image[i][j].red = 0;
				cfa_image[i][j].green = 0;
				cfa_image[i][j].blue = 0;
			}


		}
	}
	return cfa_image;
}

RGBDATA ** demosaicing_algorithm(RGBDATA ** cfa_image, int padding){
	// hyperparameters
	float alpha = 0.5;
	float beta = 0.625;
	float gamma = 0.75;

	RGBDATA ** full_color_image = createMatrix(IMAGE_WIDTH, IMAGE_HEIGHT);

	for (int i=padding;i<IMAGE_HEIGHT+padding;i++){
		for (int j=padding;j<IMAGE_WIDTH+padding;j++){
			if (i % 2 == 0 && j % 2 == 0){ // red location
				full_color_image[i-padding][j-padding].red = cfa_image[i][j].red;
				//interpolate G
				int G_b = (cfa_image[i+1][j].green + cfa_image[i-1][j].green +
					 cfa_image[i][j+1].green + cfa_image[i][j-1].green) / 4;
				int delta_R = cfa_image[i][j].red - (cfa_image[i+2][j].red +
					cfa_image[i-2][j].red + cfa_image[i][j+2].red + cfa_image[i][j-2].red) / 4;
				full_color_image[i-padding][j-padding].green = G_b + alpha * delta_R;

				//interpolate B
				int B_b = (cfa_image[i+1][j+1].blue + cfa_image[i-1][j-1].blue +
					cfa_image[i+1][j-1].blue + cfa_image[i-1][j+1].blue) / 4;
				full_color_image[i-padding][j-padding].blue = B_b + gamma * delta_R;
			}
			else if (i % 2 == 1 && i % 2 == 1){ // blue location
				full_color_image[i-padding][j-padding].blue = cfa_image[i][j].blue;
				// interpolate G
				int G_b = (cfa_image[i+1][j].green + cfa_image[i-1][j].green +
					 cfa_image[i][j+1].green + cfa_image[i][j-1].green) / 4;
				int delta_B = cfa_image[i][j].blue - (cfa_image[i+2][j].blue +
					cfa_image[i-2][j].blue + cfa_image[i][j+2].blue + cfa_image[i][j-2].blue) / 4;
				full_color_image[i-padding][j-padding].green = G_b + alpha * delta_B;

				// interpolate r
				int R_b = (cfa_image[i+1][j+1].red + cfa_image[i-1][j-1].red +
					cfa_image[i+1][j-1].red + cfa_image[i-1][j+1].red) / 4;
				full_color_image[i-padding][j-padding].red = R_b + gamma * delta_B;
			}
			else if (i % 2 == 0 && j % 2 == 1){ // green location, red row
				full_color_image[i-padding][j-padding].green = cfa_image[i][j].green;
				// interpolate R
				int R_b = (cfa_image[i][j+1].red + cfa_image[i][j-1].red) / 2;
				int delta_G_1 = cfa_image[i][j].green - 
					(cfa_image[i+1][j+1].green + cfa_image[i-1][j-1].green +
					cfa_image[i+1][j-1].green + cfa_image[i-1][j+1].green +
					cfa_image[i][j+2].green + cfa_image[i][j-2].green) / 6 +
					(cfa_image[i+2][j].green + cfa_image[i-2][j].green) / 12;

				full_color_image[i-padding][j-padding].red = R_b + beta * delta_G_1;

				// interpolate B
				int B_b = (cfa_image[i+1][j].blue + cfa_image[i-1][j].blue) / 2;
				int delta_G_2 = cfa_image[i][j].green - 
					(cfa_image[i+1][j+1].green + cfa_image[i-1][j-1].green +
					cfa_image[i+1][j-1].green + cfa_image[i-1][j+1].green +
					cfa_image[i+2][j].green + cfa_image[i-2][j].green) / 6 +
					(cfa_image[i][j+2].green + cfa_image[i][j-2].green) / 12;

				full_color_image[i-padding][j-padding].blue = B_b + beta * delta_G_2;
			}
			if (i % 2 == 1 && j % 2 == 0){

				full_color_image[i-padding][j-padding].green = cfa_image[i][j].green;
				// interpolate R
				int R_b = (cfa_image[i+1][j].red + cfa_image[i-1][j].red) / 2;
				int delta_G_2 = cfa_image[i][j].green - 
					(cfa_image[i+1][j+1].green + cfa_image[i-1][j-1].green +
					cfa_image[i+1][j-1].green + cfa_image[i-1][j+1].green +
					cfa_image[i+2][j].green + cfa_image[i-2][j].green) / 6 +
					(cfa_image[i][j+2].green + cfa_image[i][j-2].green) / 12;
				full_color_image[i-padding][j-padding].red = R_b + beta * delta_G_2;

				// interpolate B
				int B_b = (cfa_image[i][j+1].blue + cfa_image[i][j-1].blue) / 2;
				int delta_G_1 = cfa_image[i][j].green - 
					(cfa_image[i+1][j+1].green + cfa_image[i-1][j-1].green +
					cfa_image[i+1][j-1].green + cfa_image[i-1][j+1].green +
					cfa_image[i][j+2].green + cfa_image[i][j-2].green) / 6 +
					(cfa_image[i+2][j].green + cfa_image[i-2][j].green) / 12;
				full_color_image[i-padding][j-padding].blue = B_b + beta * delta_G_1;

			}

		}
	}

	return full_color_image;
}

float * calculate_PSNR(RGBDATA ** gt, RGBDATA ** output){
	float * PSNR = malloc(3 * sizeof(float));
	long long int MSE_R = 0;
	long long int MSE_G = 0;
	long long int MSE_B = 0;
	for (int i=0;i<IMAGE_HEIGHT;i++){
		for (int j=0;j<IMAGE_WIDTH;j++){
			MSE_R += pow((gt[i][j].red - output[i][j].red), 2);
		  MSE_G += pow((gt[i][j].green - output[i][j].green), 2);
			MSE_B += pow((gt[i][j].blue - output[i][j].blue), 2);
		}
	}
	MSE_R = MSE_R / (IMAGE_HEIGHT * IMAGE_WIDTH);
	MSE_G = MSE_G / (IMAGE_HEIGHT * IMAGE_WIDTH);
	MSE_B = MSE_B / (IMAGE_HEIGHT * IMAGE_WIDTH);
	PSNR[0] = 10 * log10(pow(255, 2) / MSE_R);
	PSNR[1] = 10 * log10(pow(255, 2) / MSE_G);
	PSNR[2] = 10 * log10(pow(255, 2) / MSE_B);
	return PSNR;
}

int main() {
	int NUM_OF_IMAGE = 15;
	const char *filename[15];
	filename[0] = "Kodak/1.bmp";
	filename[1] = "Kodak/2.bmp";
	filename[2] = "Kodak/3.bmp";
	filename[3] = "Kodak/4.bmp";
	filename[4] = "Kodak/5.bmp";
	filename[5] = "Kodak/6.bmp";
	filename[6] = "Kodak/7.bmp";
	filename[7] = "Kodak/8.bmp";
	filename[8] = "Kodak/9.bmp";
	filename[9] = "Kodak/10.bmp";
	filename[10] = "Kodak/11.bmp";
	filename[11] = "Kodak/12.bmp";
	filename[12] = "Kodak/13.bmp";
	filename[13] = "Kodak/14.bmp";
	filename[14] = "Kodak/15.bmp";

	printf("Calculating the PSNR values (R,G,B) of the first 15 images of the Kodak dataset...\n");

	float mean_PSNR = 0;
	for (int i=0;i<NUM_OF_IMAGE;i++){
		// Read the bmp file and retrive the 2d color table
		FILE* fp = openBmpImage(filename[i],"r");
		RGBDATA ** gt_image = readBmpDataToArr(fp);

	  // use the ground truth image to generate a CFA in a Bayer mosaic pattern
		RGBDATA ** cfa_image = gt_to_cfa(gt_image, 2);

		// Use the demosaicing algorithm to generate the interpolated full color image
		RGBDATA ** full_color_image = demosaicing_algorithm(cfa_image, 2);

		// calculate the PSNR values
		float * PSNR = calculate_PSNR(gt_image, full_color_image);
		mean_PSNR += PSNR[0] + PSNR[1] + PSNR[2];
		printf("#%d: %f %f %f\n", i+1, PSNR[0], PSNR[1], PSNR[2]);


		closeBmpImage(fp);
	}
	mean_PSNR /= (3 * NUM_OF_IMAGE);
	printf("Mean PSNR: %f", mean_PSNR);

	return 0;
}
