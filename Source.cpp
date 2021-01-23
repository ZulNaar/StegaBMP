#include <stdio.h>
#include <iostream>
#include <string>

struct Pixel {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
};

void resizeHide(std::string input, std::string output) {

	FILE* inFile = fopen(input.data(), "rb");
	if (inFile) {

		std::string message;
		unsigned char buffer[54];

		fseek(inFile, 0, SEEK_SET);
		fread(buffer, sizeof(unsigned char), 54, inFile);

		unsigned char file_signature[2] = { buffer[0], buffer[1] };

		if (file_signature[0] == 0x42 && file_signature[1] == 0x4D) {

			short int color_depth = buffer[28];

			if (color_depth == 32) {

				int file_size = buffer[2];
				int img_width = buffer[18];
				int img_height = buffer[22];
				int raw_size = buffer[34];

				int max_encode_data = 3 * (2 * img_width + 2 * img_height + 4);

				printf("Enter the text that you want to hide in the image:\n\n");
				std::getline(std::cin, message);
				printf("\n");

				if (message.size() <= max_encode_data) {
					printf("Message size(%d bytes) is less or equal to the maximum encodable data on the border pixels(%d bytes)\n - OK - \n", message.size(), max_encode_data);

					FILE* outFile = fopen(output.data(), "wb");
					if (outFile) {

						//modifying some of the header and infoheader info in order to resize the image and still be able to open it

						int new_img_width = img_width + 2;
						int new_img_height = img_height + 2;
						int new_raw_size = new_img_width * new_img_height * 4;
						int new_file_size = new_raw_size + 54;

						buffer[2] = new_file_size;
						buffer[18] = new_img_width;
						buffer[22] = new_img_height;
						buffer[34] = new_raw_size;

						fwrite(buffer, sizeof(unsigned char), 54, outFile);

						const char* data = message.data();
						int k = message.length();

						Pixel* pixel = new Pixel;
						Pixel* pixelHelper = new Pixel;

						for (int m = 0; m < new_img_height; m++)
							for (int n = 0; n < new_img_width; n++) {
								if (m == 0 || n == 0 || m == (new_img_height - 1) || n == (new_img_width - 1)) { //new image border operations
									pixelHelper->B = 0; pixelHelper->G = 0; pixelHelper->R = 0; pixelHelper->A = 0;
									if (k) {
										pixelHelper->B = data[message.size() - k];
										k--;
									}
									if (k) {
										pixelHelper->G = data[message.size() - k];
										k--;
									}
									if (k) {
										pixelHelper->R = data[message.size() - k];
										k--;
									}
									fwrite(pixelHelper, sizeof(unsigned char), 4, outFile);
								}
								else { //original image pixels
									fread(pixel, sizeof(unsigned char), 4, inFile);
									fwrite(pixel, sizeof(unsigned char), 4, outFile);
								}
							}
						fclose(outFile);
						printf("Task completed! Output file %s created\n", output.data());

						delete pixel; pixel = NULL;
						delete pixelHelper; pixelHelper = NULL;
					}
					else
						printf("Unable to create output file %s. Aborting...\n", output.data());
				}
				else {
					printf("Message size(%d bytes) is greater than the maximum encodable data on the border pixels(%d bytes)\n - NOT OK - \n", message.size(), max_encode_data);
					printf("Aborting...\n");
				}
			}
			else
				printf("The color depth of input bitmap file(%hd bits per pixel) is not 32 bpp. Aborting...\n", color_depth);
		}
		else
			printf("The specified input file is not a Bitmap. Aborting...\n");
		fclose(inFile);
	}
	else
		printf("Unable to open file %s. Aborting...\n", input.data());
}

void unCover(std::string input) {
	Pixel* pixel = new Pixel;
	unsigned char buffer[54];

	FILE* rdFile = fopen(input.data(), "rb");
	if (rdFile) {

		fseek(rdFile, 0, SEEK_SET);
		fread(buffer, sizeof(unsigned char), 54, rdFile);

		unsigned char file_signature[2] = { buffer[0], buffer[1] };

		if (file_signature[0] == 0x42 && file_signature[1] == 0x4D) {

			short int color_depth = buffer[28];

			if (color_depth == 32) {

				int img_width = buffer[18];
				int img_height = buffer[22];

				printf("Uncovering hidden message:\n\n");

				for (int m = 0; m < img_width; m++)
					for (int n = 0; n < img_height; n++) {
						fread(pixel, sizeof(unsigned char), 4, rdFile);
						if (m == 0 || n == 0 || m == (img_height - 1) || n == (img_width - 1)) {
							if (pixel->B)
								printf("%c", pixel->B);
							if (pixel->G)
								printf("%c", pixel->G);
							if (pixel->R)
								printf("%c", pixel->R);
						}
					}
			}
			else
				printf("The color depth of input bitmap file(%hd bits per pixel) is not 32 bpp. Aborting...\n", color_depth);
		}
		else
			printf("The specified input file is not a Bitmap. Aborting...\n");
		fclose(rdFile);
		printf("\n");
	}
	else
		printf("Unable to open file %s. Aborting...\n", input.data());

	delete pixel; pixel = NULL;
}
	
void main() {
	std::string input, output, secret, selection;

	printf("Specify 'H' if you want to hide a message or 'U' if you want to uncover one\n");
	std::getline(std::cin, selection);
	printf("\n");
	if (selection.length() != 1) {
		printf("Invalid operation. Aboring...\n");
	}
	else {
		if (selection.data()[0] == 'h' || selection.data()[0] == 'H') {
			printf("Image in which the message will be hidden:\n");
			std::getline(std::cin, input);
			printf("\n");
			printf("Resulting output image, containing hidden message:\n");
			std::getline(std::cin, output);
			printf("\n");
			resizeHide(input, output);
		}
		else if (selection.data()[0] == 'u' || selection.data()[0] == 'U') {
			printf("Enter the name of the file from which you want to uncover the secret message:\n");
			std::getline(std::cin, secret);
			printf("\n");
			unCover(secret);
		}
		else
			printf("Invalid operation. Aboring...\n");
	}
	
}