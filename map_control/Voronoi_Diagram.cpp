#include "Voronoi_Diagram.h"
// -------------------------------------------------------------------------
Voronoi_Diagram::Voronoi_Diagram() {}
// -------------------------------------------------------------------------
Voronoi_Diagram::~Voronoi_Diagram() {}
// -------------------------------------------------------------------------
void Voronoi_Diagram::get_voronoi_img( const cv::Mat &src, cv::Mat &dst ) {
    voronoi( src, dst );
}
// -------------------------------------------------------------------------
void Voronoi_Diagram::get_thinning_img( const cv::Mat &src, cv::Mat &dst ) {
    opencv_thinning( src, dst );
}
// -------------------------------------------------------------------------
void Voronoi_Diagram::get_skeletinize_img( const cv::Mat &src, cv::Mat &dst) {
    skeletinize( src, dst );
}
// -------------------------------------------------------------------------

/***********
 * Private
 **********/
// -------------------------------------------------------------------------
void Voronoi_Diagram::voronoi( const cv::Mat &input,
                               cv::Mat &output_img )
{
    Mat gray;
    cvtColor( input, gray, CV_BGR2GRAY );
    threshold( gray, gray, 127, 1, CV_THRESH_BINARY );
    for (int y = 0; y < gray.rows; y++) {
        gray.at<uchar>(y, 0) = 0;
        gray.at<uchar>(y, gray.cols-1) = 0;
    }
    for (int x = 0; x < gray.cols; x++) {
        gray.at<uchar>(0, x) = 0;
        gray.at<uchar>(gray.rows-1, x) = 0;
    }
    make_voronoi( gray, output_img );
}
// -------------------------------------------------------------------------
void Voronoi_Diagram::opencv_thinning(const cv::Mat &input,
                               cv::Mat &output_img )
{
    Mat gray;
    cv::cvtColor( input, gray, CV_BGR2GRAY );
    cv::threshold( gray, gray, 127, 255, CV_THRESH_BINARY );
    for (int y = 0; y < gray.rows; y++) {
        gray.at<uchar>(y, 0) = 0;
        gray.at<uchar>(y, gray.cols-1) = 0;
    }
    for (int x = 0; x < gray.cols; x++) {
        gray.at<uchar>(0, x) = 0;
        gray.at<uchar>(gray.rows-1, x) = 0;
    }
    cv::ximgproc::thinning( gray, output_img, cv::ximgproc::THINNING_ZHANGSUEN );
}
// -------------------------------------------------------------------------
void Voronoi_Diagram::skeletinize( const cv::Mat &input,
                                   cv::Mat &output_img )
{
    Mat gray;
    cv::cvtColor( input, gray, CV_BGR2GRAY );
    cv::threshold( gray, gray, 127, 255, CV_THRESH_BINARY );
    for (int y = 0; y < gray.rows; y++) {
        gray.at<uchar>(y, 0) = 0;
        gray.at<uchar>(y, gray.cols-1) = 0;
    }
    for (int x = 0; x < gray.cols; x++) {
        gray.at<uchar>(0, x) = 0;
        gray.at<uchar>(gray.rows-1, x) = 0;
    }
    Mat skel( gray.size(), CV_8UC1, Scalar(0) ), temp, eroded, element;
    element = cv::getStructuringElement( MORPH_CROSS, Size(3,3) );
    bool done = false;
    do {
        erode( gray, eroded, element );
        dilate( eroded, temp, element );
        subtract( gray, temp, temp );
        bitwise_or( skel, temp, skel );
        eroded.copyTo( gray );
        done = ( cv::countNonZero( gray ) == 0 );
    }
    while ( done == false );
    output_img = skel.clone();
}
// -------------------------------------------------------------------------
void Voronoi_Diagram::thin_subiteration_1( const cv::Mat &input, cv::Mat &output ) {
    input.copyTo( output );

    for (int y = 1; y < input.rows; y++)
        for (int x = 1; x < input.cols; x++) {
            if ( input.at<float>(y,x) == 1.0f ) {
                int neighbor_0 = (int)input.at<float>( y-1, x-1 );
                int neighbor_1 = (int)input.at<float>( y-1, x );
                int neighbor_2 = (int)input.at<float>( y-1, x+1 );
                int neighbor_3 = (int)input.at<float>( y, x+1 );
                int neighbor_4 = (int)input.at<float>( y+1, x+1 );
                int neighbor_5 = (int)input.at<float>( y+1, x );
                int neighbor_6 = (int)input.at<float>( y+1, x-1 );
                int neighbor_7 = (int)input.at<float>( y, x-1 );

                int c = int( ~neighbor_1 & ( neighbor_2 | neighbor_3 ) ) +
                        int( ~neighbor_3 & ( neighbor_4 | neighbor_5 ) ) +
                        int( ~neighbor_5 & ( neighbor_6 | neighbor_7 ) ) +
                        int( ~neighbor_7 & ( neighbor_0 | neighbor_1 ) );

                if ( c == 1 ) {
                    int n_1 =   int( neighbor_0 | neighbor_1 ) +
                                int( neighbor_2 | neighbor_3 ) +
                                int( neighbor_4 | neighbor_5 ) +
                                int( neighbor_6 | neighbor_7 );

                    int n_2 =   int( neighbor_1 | neighbor_2 ) +
                                int( neighbor_3 | neighbor_4 ) +
                                int( neighbor_5 | neighbor_6 ) +
                                int( neighbor_7 | neighbor_0 );

                    int n = min( n_1, n_2 );

                    if ( n == 2 || n == 3 ) {
                        int c_3 = ( neighbor_1 | neighbor_2 | ~neighbor_4 ) & neighbor_3;

                        if ( c_3 == 0 ) {
                            output.at<float>(y,x) = 0.0f;
                        }
                    }
                }
            }
        }
}
// -------------------------------------------------------------------------
void Voronoi_Diagram::thin_subiteration_2( const cv::Mat &input, cv::Mat &output ) {
    input.copyTo( output );
    for (int y = 1; y < input.rows; y++)
        for (int x = 1; x < input.cols; x++) {
            if ( input.at<float>(y,x) == 1.0f ) {
                int neighbor_0 = (int)input.at<float>( y-1, x-1 );
                int neighbor_1 = (int)input.at<float>( y-1, x );
                int neighbor_2 = (int)input.at<float>( y-1, x+1 );
                int neighbor_3 = (int)input.at<float>( y, x+1 );
                int neighbor_4 = (int)input.at<float>( y+1, x+1 );
                int neighbor_5 = (int)input.at<float>( y+1, x );
                int neighbor_6 = (int)input.at<float>( y+1, x-1 );
                int neighbor_7 = (int)input.at<float>( y, x-1 );

                int c = int( ~neighbor_1 & ( neighbor_2 | neighbor_3 ) ) +
                        int( ~neighbor_3 & ( neighbor_4 | neighbor_5 ) ) +
                        int( ~neighbor_5 & ( neighbor_6 | neighbor_7 ) ) +
                        int( ~neighbor_7 & ( neighbor_0 | neighbor_1 ) );

                if ( c == 1 ) {
                    int n_1 =   int( neighbor_0 | neighbor_1 ) +
                                int( neighbor_2 | neighbor_3 ) +
                                int( neighbor_4 | neighbor_5 ) +
                                int( neighbor_6 | neighbor_7 );

                    int n_2 =   int( neighbor_1 | neighbor_2 ) +
                                int( neighbor_3 | neighbor_4 ) +
                                int( neighbor_5 | neighbor_6 ) +
                                int( neighbor_7 | neighbor_0 );

                    int n = min( n_1, n_2 );

                    if ( n == 2 || n == 3 ) {
                        int e = ( neighbor_5 | neighbor_6 | ~neighbor_0 ) & neighbor_7;

                        if ( e == 0 )
                            output.at<float>(y,x) = 0.0f;
                    }
                }
            }
        }
}
// -------------------------------------------------------------------
void Voronoi_Diagram::make_voronoi( cv::Mat &input, cv::Mat &output) {
    bool done = false;
    input.convertTo( input, CV_32FC1 );
    input.copyTo( output );
    output.convertTo( output, CV_32FC1 );

    // start to thin
    Mat thin_mat_1 = Mat::zeros( input.rows, input.cols, CV_32FC1 );
    Mat thin_mat_2 = Mat::zeros( input.rows, input.cols, CV_32FC1 );
    Mat cmp = Mat::zeros( input.rows, input.cols, CV_8UC1 );

    while ( done != true ) {
        thin_subiteration_1( output, thin_mat_1 );      // sub-iteration 1
        thin_subiteration_2( thin_mat_1, thin_mat_2 );  // sub-iteration 2
        compare( output, thin_mat_2, cmp, CV_CMP_EQ );  // compare
        int num_non_zero = countNonZero( cmp );         // check
        if ( num_non_zero == ( input.rows * input.cols ) )
            done = true;
        thin_mat_2.copyTo( output );
    }
}
