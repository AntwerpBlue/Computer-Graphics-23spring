#include "SaliencyDetect.h"
#include <iostream>
#include <fstream>

#define K 64
#define C 3

using std::ofstream;
using std::cout;
using std::endl;
using std::pair;
using namespace cv;

Mat source;
int rows, cols;

double color_distance(const Mat & src, int row1, int col1, int row2, int col2) {
    auto & c1 = src.at<Vec3b>(row1, col1), & c2 = src.at<Vec3b>(row2, col2);
    double dc0 = (c1[0] - c2[0] + 0.0) / 255, dc1 = (c1[1] - c2[1] + 0.0) / 255, dc2 = (c1[2] - c2[2] + 0.0) / 255;
    //note that the dist in the color space should be normalized into range [0,1], exactly as in the paper
    return sqrt(dc0 * dc0 + dc1 * dc1 + dc2 * dc2);
}

double distance(const Mat & src, int row1, int col1, int row2, int col2) {
    double color_dis = color_distance(src, row1, col1, row2, col2);
    double dRow = (row1 - row2 + 0.0) / rows, dCol = (col1 - col2 + 0.0) / cols;
    double xy_dis = sqrt(dRow * dRow + dCol * dCol);
    return color_dis / (1 + C * xy_dis);//that is the equation(1) in the paper
}

double salient(const Mat & src, int r, int c) {
    //the function returns the saliency value of pixel(r,c) 
    std::vector<double> diffs;
    for (int row = 0; row < rows; ++row)
        for (int col = 0; col < cols; ++col)
            diffs.push_back(distance(src, r, c, row, col));
            //diffs stores the dissimilarity between given (r,c) and all other patches
    std::sort(diffs.begin(), diffs.end());
    double sum = 0;
    int n = 0;
    for (n = 0; n <= K && n < diffs.size(); ++n)//just coonsider the K most similar patches, and K is defined as in the paper
        sum += diffs[n];
    return 1 - exp(-sum/n);//that is the equation(2) in the paper
}

Mat saliencyMatrix(const Mat & _src, int u) {//u is the size of the patch for each pixel
    //the function returns a matrix containing saliency values of _src, and in size of u
    Mat src = _src.clone();
    parallel_for_(Range(0, rows), [&](const Range& r)
        {
            for (int row = 0; row < rows; ++row) {
                for (int col = 0; col < cols; ++col) {
                    int n = 0;
                    int l = 0, a = 0, b = 0;
                    for (int r = row - u; r <= row + u; ++r) {
                        if (r < 0 || r >= rows) continue;
                        for (int c = col - u; c <= col + u; ++c) {
                            if (c < 0 || c >= cols) continue;
                            ++n;
                            l += _src.at<Vec3b>(r, c)[0];//*****should there be _src instead of src?
                            a += _src.at<Vec3b>(r, c)[1];
                            b += _src.at<Vec3b>(r, c)[2];
                        }
                    }
                    src.at<Vec3b>(row, col) = Vec3b(static_cast<uchar>(l / n), static_cast<uchar>(a / n), static_cast<uchar>(b / n));
                }//using the average of all pixels in the patch, with size of 2u*2u, as the color of the pixel
            }});
    Mat tg = Mat(rows, cols, CV_8U);
    Mat mid = Mat(rows, cols, CV_64F);
    double _max = 0;
    printf("generating at u=%d\n", u);
    for (int row = 0; row < rows; ++row) {
        printf("\r%d/%d", row, rows);
        std::flush(std::cout);
        for (int col = 0; col < cols; ++col) {
            double value = salient(src, row, col);
            mid.at<double>(row, col) = value;//mid stores the saliency values of all the pixels
            if (value > _max) _max = value;//_max of the saliency values
        }
    }
    parallel_for_(Range(0, rows), [&](const Range& r)
        {
            for (int row = 0; row < rows; ++row) {
                for (int col = 0; col < cols; ++col) {
                    double value = mid.at<double>(row, col) / _max;//normalized into range [0,255]
                    tg.at<uchar>(row, col) = static_cast<uchar>(value * 255);
                }
            }});
    printf("\n");
    return tg;
}

void exec(const String &filename, const String& outFile, int u) {
    source = imread(filename);
    cvtColor(source, source, cv::ColorConversionCodes::COLOR_RGB2Lab);//convert the source image into RGB color space
    rows = source.rows;
    cols = source.cols;
    printf("rows: %d, cols: %d\n", rows, cols);

    Mat tg4 = saliencyMatrix(source, u);
    imwrite(filename + ".4.png", tg4);
    Mat tg2 = saliencyMatrix(source, u/2);
    imwrite(filename + ".2.png", tg2);
    Mat tg0 = saliencyMatrix(source, 0);
    imwrite(filename + ".0.png", tg0);
    Mat tg = Mat(rows, cols, CV_8U);


    //the following is multi-scale enchancement
    std::vector<pair<pair<int, int>, uchar>> mainPart;//contains the attended pixels, storing the coordinate and value
    uchar _max = 0;
    parallel_for_(Range(0, rows), [&](const Range& r)
        {
            for (int row = 0; row < rows; ++row) {
                for (int col = 0; col < cols; ++col) {
                    auto avg = (uchar)(
                        ((int)tg4.at<uchar>(row, col) + (int)tg2.at<uchar>(row, col) + (int)tg0.at<uchar>(row, col)) / 3);
                    //that is equation(4), noting that the type should be converted during the operation
                    tg.at<uchar>(row, col) = avg;
                    if (avg > _max) _max = avg;
                }
            }});
    printf("tg max: %d", _max);
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            tg.at<uchar>(row, col) = static_cast<uchar>(tg.at<uchar>(row, col) * 255.0 / _max);
            if (tg.at<uchar>(row, col) > 204) mainPart.emplace_back(pair<int, int>(row, col), tg.at<uchar>(row, col));
        }//for those saliency value larger than 80%, including immediate context
    }
    imwrite(filename + ".tg.png", tg);


    //the following considers immediate context
    Mat S = tg.clone();
    printf("len:%d\n", static_cast<int>(mainPart.size()));
    printf("optimizing\n");
    if (!mainPart.empty()) {
        for (int row = 0; row < rows; ++row) {
            printf("\r%d/%d", row, rows);
            std::flush(std::cout);
            for (int col = 0; col < cols; ++col) {
                uchar value = S.at<uchar>(row, col);
                if (value > 204) continue;
                double dis = 1;
                for (auto p: mainPart) {
                    double dRow = (p.first.first - row + 0.0) / rows;
                    double dCol = (p.first.second - col + 0.0) / cols;
                    double _dis = sqrt(dRow * dRow + dCol * dCol);
                    if (_dis < dis) dis = _dis;//find the nearest foci
                }
                S.at<uchar>(row, col) = static_cast<uchar>(value * (1.0 - dis));//that is equation(5)
            }
        }
    }
    printf("\n");
    imwrite(filename + ".fin.png", S);
    imwrite(outFile, S);
}