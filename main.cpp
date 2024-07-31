#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <vector>

#include "quadtree_2.hpp"

int main(int argc, char **argv) {

  cv::Point2i p1(0, 0);
  std::vector<cv::Point2i> polygon = {cv::Point2i(10, 10), cv::Point2i(20, 10),
                                      cv::Point2i(20, 20), cv::Point2i(10, 20)};

  QuadTree<int, std::vector<cv::Point2i> *> qt;

  qt.setRoot({0, 0}, {100, 100});
  qt.insert(&polygon);

    cv::Point2i testPoint(15, 15);
    cv::Point2i testPoint_1(25, 25);
//  double result = cv::pointPolygonTest(polygon, testPoint, false);
//  double result_1 = cv::pointPolygonTest(polygon, testPoint_1, false);

    const std::vector<std::vector<cv::Point2i> *> * polygon_tst =  qt.query(testPoint);
    if(polygon_tst != nullptr) {
        std::cout << "GOOD" << std::endl;
    } else {
        std::cout << "BAD" << std::endl;
    }
    const std::vector<std::vector<cv::Point2i> *> *  polygon_tst_1 =  qt.query(testPoint_1);
    if(polygon_tst_1 != nullptr) {
        std::cout << "BAD" << std::endl;
    } else {
        std::cout << "GOOD" << std::endl;
    }

  std::cout << "hello open cv " << std::endl;
  return 0;
}