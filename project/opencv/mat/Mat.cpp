/**
 * Mat a class has two data parts:
 *  1.the matrix header(size of the matrix,method ,for stroring,at which address the matrix stored)
 *  2. a pointer to matrix containing the pixel values
 *
 *  the matrix header size is constant,the size of matrix varies form image to image ,always larger by orders of magnitude
 *
 *  unnescessary copies of potentially large images:use a reference counting system.
 *   each Mat object has its own header,the matrix may be shared between two instance of them by having their matrix pointers points to the same address.Moreover,the copy operators will only copy the headers and the pointer to the large matrix,not the data itself   
 * */

#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

using namespace std;
int createMatObjectExplicitly();
int outFormat();
int outputOtherCommonItems();
int main(int argc,char* argv[]){

    argv[1]="../../data/lena.jpg";
    Mat A, C; //creates just hte header parts
    A=imread(argv[1],IMREAD_COLOR);//here we'll know the method used (allocate matrix)
    Mat B(A);       //use the copy constructor
    C = A;          //assignment operator
    
    /*
     * TODO all above objects,point to the same single data matrix(their headers are different)
     *
     * you can create headers which refer to only a subsection of the full data.
     * Such,Create a region of interest(ROI) in an image you just create a new header with the new boundaries
     * */
    Mat D (A,Rect(10,10,100,100)); //use a rectangle
    Mat E = A(Range::all(),Range(1,3));//using row and column bondaries

    /**
     * if the matrix itself may belong to multiple Mat objects who takes reponsibility for cleaning it up when it's no longer needed?
     * TODO the alst object that used it.
     * Whenever somebody copies a header of a Mat object, a counter is increased for the matrix.Whenever a header is cleaned this counter is decreased.When the counter reaches zero the matrix too is freed.
     *
     * TODO sometimes you will want to copy the matrix itself too,so OpenCV provides the cv::Mat::clone() and cv::Mat::copyTo() functions.
     * */
    Mat F = A.clone();
    Mat G;
    A.copyTo(G);

    /**
     * now modifying F or G will not affect the matrix pointed by the Mat header.What you need to remember from this that:
     *  Output image allocation for OpenCV funcitons is automatic
     *  You do not need think about memory management with OpenCVs C++ interface
     *  The assignment operator and the copy constructor only copies the header
     *  The underling matrix of an image may be copies using the cv::Mat::clone() and cv::Mat::copyTo()
     * */

    createMatObjectExplicitly();
    outFormat();
    outputOtherCommonItems();
}

/**TODO BGR OpenCV used
 * << operator :this only works for two dimensional matrices
 * */
int createMatObjectExplicitly() {
    //1.cv::Mat::Mat Constructor
    Mat M(2,2,CV_8UC3,Scalar(0,0,255));
    cout << "M = "<<endl << " " << M <<endl <<endl;

    //2.c/c++arrays and initalize via constructor
    int sz[3]= {2,2,2};
    Mat L(3,sz,CV_8UC(1),Scalar::all(0));

    //3.cv::Mat::create function
    M.create(4,4,CV_8UC(2));
    cout<<"M="<<endl<<" "<<M<<endl<<endl;
    //4.MATLAB style
    Mat E = Mat::eye(4,4,CV_64F);
    cout<<"E= "<<endl<<" "<<E<<endl<<endl;
    Mat O=Mat::ones(2,2,CV_32F);
    cout<<"O= "<<endl<<" "<<O<<endl<<endl;
    Mat Z = Mat::zeros(3,3,CV_8UC1);
    cout<<"Z ="<<endl<<" "<<Z<<endl<<endl;

    //5.small matrices you may use comma separated inializers or initializer lists ,c++ needed
    //Mat C =(Mat<double>(3,3)<<0,-1,0,-1,5,-1,0,-1,0);
    //cout<<"C ="<<endl<< " "<<C<<endl<<endl;
    //C=(Mat<double>({0,-1,0,-1,5,-1,0,-1,0})).reshape(3);
    //cout<<"C ="<<endl<<" "<<C<<endl<<endl;
    
    //6.create a new header for an existing Mat object
    Mat RowClone = E.row(1).clone();
    cout<<"RowClone="<<endl<<" "<<RowClone<<endl<<endl;

    //randu()
    Mat R=Mat(3,2,CV_8UC3);
    randu(R,Scalar::all(0),Scalar::all(255));
}

int outFormat() {
    Mat E = Mat::eye(4,4,CV_64F);
    //1.default
    cout<<"E(default) = "<<endl<<E<<endl<<endl;

    //2.python
    cout<<"E (python) ="<<endl <<format(E,Formatter::FMT_PYTHON)<<endl<<endl;
    //3.CSV
    cout <<"E (CSV) ="<<endl<<format(E,Formatter::FMT_NUMPY)<<endl<<endl;
    //4.numpy
    cout <<"E (CSV) ="<<endl<<format(E,Formatter::FMT_NUMPY)<<endl<<endl;
    //5.C
    cout <<"E (C) ="<<endl<<format(E,Formatter::FMT_C)<<endl<<endl;
}
int outputOtherCommonItems() {
    //1.Point2D
    Point2f P(5,1);
    cout<<"point (2D) ="<<P<<endl<<endl;
    //2.3D Point
    Point3f P3f(2,6,7);
    cout <<"point (3D) = "<<P3f <<endl<<endl;
    //3.std::vector
    vector<float> v;
    v.push_back((float)CV_PI);
    v.push_back(2);
    v.push_back(3.01f);
    cout<<"Vector of floats via Mat ="<<Mat(v)<<endl<<endl;
    //4.std::vector of points
    vector<Point2f> vPoints(20);
    for(size_t i=0;i<vPoints.size();++i){
        vPoints[i] = Point2f((float)(i*5),(float)(i%7));
    }
    cout<<"A vector of 2D Points="<<vPoints<<endl<<endl;
}
