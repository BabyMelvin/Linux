#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <string>
using namespace cv;
using namespace std;

int main(int argc,char * argv[]){
    String imageName("../../data/HappyFish.jpg"); 
    if(argc >1){
        imageName=argv[1];
    }
    Mat image;
    /*
     *IMREAD_UNCHANGED(<0):loads the image as is(include the alpha channel if present)
     *IMREAD_GRAYSCALE(0):loads the image as an intensity one
     *IMREAD_COLOR(>0):loads the image in the RGB format
     */ 

    image=imread(imageName,IMREAD_COLOR);
    if(image.empty()){
        cout << "could not open or find the image"<<endl;
        return -1;
    }
    namedWindow("Display window",WINDOW_AUTOSIZE);
    imshow("Display window",image);
    waitKey(0);
    return 0;
}
