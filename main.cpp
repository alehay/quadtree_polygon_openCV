#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <vector>

#include "quadtree.hpp"

int main(int argc, char **argv) {

  cv::Point2i p1(0, 0);
  std::vector<cv::Point2i> polygon = {cv::Point2i(0, 0), cv::Point2i(10, 0),
                                      cv::Point2i(10, 10), cv::Point2i(0, 10)};

  QuadTree<int, std::vector<cv::Point2i> *> qt;

  qt.setRoot({0, 0}, {1000, 1000});
  qt.insert(&polygon);

    cv::Point2i testPoint(0, 0);
    cv::Point2i testPoint_1(25, 25);
  double result = cv::pointPolygonTest(polygon, testPoint, false);
    std::cout << "test resul ->" << result << std::endl;
//  double result_1 = cv::pointPolygonTest(polygon, testPoint_1, false);

    std::vector<cv::Point2i> * polygon_tst =  qt.getPointerPolygon(testPoint);
    if(polygon_tst != nullptr) {
        std::cout << "GOOD" << std::endl;
    } else {
        std::cout << "BAD" << std::endl;
    }
    std::vector<cv::Point2i> *  polygon_tst_1 =   qt.getPointerPolygon(testPoint_1);
    if(polygon_tst_1 != nullptr) {
        std::cout << "BAD" << std::endl;
    } else {
        std::cout << "GOOD" << std::endl;
    }

  std::cout << "hello open cv " << std::endl;
  return 0;
}