/*
 * Created on: Jul 5, 2014
 * Author: Chris Garry
 *
 * Performs histogram equalization on all images
 * in source directory (argv[1]) and store results
 * in destination directory (argv[2]). Source and
 * destination directories must contain a trailing
 * slash.
 *
 *	Usage:
 *	./batchEqualizeHist ../images/unprocessed/ ../images/processed/
 *
 */

#include <dirent.h>
#include <sys/errno.h>
#include <cstdio>
#include <string>
#include <vector>

#include "/usr/local/Cellar/opencv/2.4.8.2/include/opencv2/imgproc/imgproc.hpp"
#include "/usr/local/Cellar/opencv/2.4.8.2/include/opencv2/opencv.hpp"

//Performs histogram equalization on a single color image
cv::Mat colorEqualizeHist(cv::Mat image){

	cv::Mat img_ycrcb;
	std::vector<cv::Mat> channels;

	//Convert to channel-independent intensity representation ycrcb
	cv::cvtColor(image,img_ycrcb,CV_BGR2YCrCb);
	cv::split(img_ycrcb,channels);

	//Perform equalizeHist on intensity plane and reconstruct color channels
	cv::equalizeHist(channels[0], channels[0]);
	cv::merge(channels,img_ycrcb);
	cv::cvtColor(img_ycrcb, image, CV_YCrCb2BGR, 0);

	return image;

}

//Validates arguments for batch equalization
int validateArgs(int argc, char *argv[]){

	//Validate that there are only two user inputs
	if(argc != 3){
		printf("Error: Invalid argument count. Please provide a source "
				"and destination directory.\n");
		return 1;
	}

	//Validate format of inputs
	std::string src(argv[1]);
	std::string dest(argv[2]);
	if(src.at(src.length()-1) != '/' || dest.at(dest.length()-1) != '/'){
		printf("Error: Invalid directory. Directory must end with a "
				"trailing slash.\n");
		return 1;
	}

	return 0;
}

//Batch histogram equalization on color images
int main(int argc, char* argv[]){

	if(validateArgs(argc, argv))
		return 1;

	cv::Mat image;
	std::string filepath;
	DIR *source_dir = NULL;
	dirent *dp = NULL;

	//Create directory struct and read first file
	if((source_dir = opendir(argv[1])) == NULL){
		printf("Error: Source directory does not exist. Errorno %d.\n", errno);
		return 1;
	}

	dp = readdir(source_dir);

	//For each file in the directory
	while(true){

		//Check if end of directory
		if (!dp)
			break;

		//Build full file path
		filepath.clear();
		filepath.append(argv[1]);
		filepath.append(dp->d_name);

		//Attempt to read file as image from filepath
		image = cv::imread(filepath, CV_LOAD_IMAGE_COLOR);

		//If file is a supported image type, process it
		if(image.empty() == 0){

			image = colorEqualizeHist(image);
			filepath.clear();
			filepath.append(argv[2]);
			filepath.append(dp->d_name);
			cv::imwrite(filepath, image);
		}

		//Read next file in directory
		dp = readdir(source_dir);

	}

	closedir(source_dir);
	return 0;
}
