#include "quadtree.hpp"
#include "rtree.hpp"
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include <vector>

// Function to generate random polygons
std::vector<cv::Point2i> generateRandomPolygon(int minX, int maxX, int minY,
                                               int maxY, int numPoints) {
  std::vector<cv::Point2i> polygon;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> disX(minX, maxX);
  std::uniform_int_distribution<> disY(minY, maxY);

  for (int i = 0; i < numPoints; ++i) {
    polygon.emplace_back(disX(gen), disY(gen));
  }
  return polygon;
}

cv::Scalar getColorFromNumbers(int number1, int number2)

{
  std::size_t h1 = std::hash<int>{}(number1);
  std::size_t h2 = std::hash<int>{}(number2);
  std::size_t res = h1 ^ (h2 << 1);

  int r = (h1 % 128) + 128;
  int g = (h2 % 128) + 128;
  int b = (res % 128) + 128;

  return cv::Scalar(b, g, r);
}

bool polygonsIntersect(const std::vector<cv::Point2i>& poly1, const std::vector<cv::Point2i>& poly2) {
    std::vector<cv::Point2f> poly1f(poly1.begin(), poly1.end());
    std::vector<cv::Point2f> poly2f(poly2.begin(), poly2.end());
    cv::Mat intersectingRegion;
    return cv::rotatedRectangleIntersection(cv::minAreaRect(poly1f), cv::minAreaRect(poly2f), intersectingRegion) != cv::INTERSECT_NONE;
}


std::vector<cv::Point2i> generateSmallRandomRectangle(int minX, int maxX, int minY, int maxY, int maxSize, const std::vector<std::vector<cv::Point2i>>& existingPolygons) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(minX, maxX - maxSize);
    std::uniform_int_distribution<> disY(minY, maxY - maxSize);
    std::uniform_int_distribution<> disSize(10, maxSize);

    for (int attempts = 0; attempts < 100; ++attempts) {
        int topLeftX = disX(gen);
        int topLeftY = disY(gen);
        int width = disSize(gen);
        int height = disSize(gen);

        std::vector<cv::Point2i> rectangle = {
            {topLeftX, topLeftY},
            {topLeftX + width, topLeftY},
            {topLeftX + width, topLeftY + height},
            {topLeftX, topLeftY + height}
        };

        bool intersects = false;
        for (const auto& existingPolygon : existingPolygons) {
            if (polygonsIntersect(rectangle, existingPolygon)) {
                intersects = true;
                break;
            }
        }

        if (!intersects) {
            return rectangle;
        }
    }

    // If we couldn't generate a non-intersecting polygon after 100 attempts, return an empty vector
    return {};
}



// Function to measure execution time
template <typename Func> double measureExecutionTime(Func func) {
  auto start = std::chrono::high_resolution_clock::now();
  func();
  auto end = std::chrono::high_resolution_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
             .count() /
         1000.0;
}

// Brute force method to check if a point is inside a polygon
bool isPointInPolygon(const cv::Point2i &point,
                      const std::vector<cv::Point2i> &polygon) {
  return cv::pointPolygonTest(polygon, point, false) >= 0;
}

// Brute force method to find the polygon containing a point
std::vector<cv::Point2i> *
bruteForceQuery(const cv::Point2i &point,
                const std::vector<std::vector<cv::Point2i>> &polygons) {
  for (auto &polygon : polygons) {
    if (isPointInPolygon(point, polygon)) {
      return const_cast<std::vector<cv::Point2i> *>(&polygon);
    }
  }
  return nullptr;
}

int main() {
  const int NUM_POLYGONS = 1 << 12;
  const int NUM_QUERIES = 1 << 15;
  const int WORLD_SIZE = 1 << 11;

    std::vector<std::vector<cv::Point2i>> polygons;
    for (int i = 0; i < NUM_POLYGONS; ++i) {
        auto newPolygon = generateSmallRandomRectangle(0, WORLD_SIZE, 0, WORLD_SIZE, 100, polygons);
        if (!newPolygon.empty()) {
            polygons.push_back(newPolygon);
        }
    }

  QuadTree<int, std::vector<cv::Point2i> *> qt;
  RTree<int, std::vector<cv::Point2i> *> rt;

  // Measure insertion time
  double qtInsertTime = measureExecutionTime([&]() {
    qt.setRoot({0, 0}, {WORLD_SIZE, WORLD_SIZE});
    for (auto &polygon : polygons) {
      qt.insert(&polygon);
    }
  });

  double rtInsertTime = measureExecutionTime([&]() {
    for (auto &polygon : polygons) {
      rt.insert(&polygon);
    }
  });

  std::cout << "QuadTree insertion time: " << qtInsertTime << " ms\n";
  std::cout << "RTree insertion time: " << rtInsertTime << " ms\n";

  // Generate query points
  std::vector<cv::Point2i> queryPoints;
  for (int i = 0; i < NUM_QUERIES; ++i) {
    queryPoints.emplace_back(rand() % WORLD_SIZE, rand() % WORLD_SIZE);
  }

  // Measure query time
  double qtQueryTime = measureExecutionTime([&]() {
    for (const auto &point : queryPoints) {
      volatile std::vector<cv::Point2i> *vp =
          static_cast<volatile std::vector<cv::Point2i> *>(
              qt.getPointerPolygon(point));
    }
  });

  double rtQueryTime = measureExecutionTime([&]() {
    for (const auto &point : queryPoints) {
      volatile std::vector<cv::Point2i> *vp =
          static_cast<volatile std::vector<cv::Point2i> *>(
              rt.getPointerPolygon(point));
    }
  });

  // Measure brute force query time
  double bfQueryTime = measureExecutionTime([&]() {
    for (const auto &point : queryPoints) {
      volatile std::vector<cv::Point2i> *vp =
          static_cast<volatile std::vector<cv::Point2i> *>(
              bruteForceQuery(point, polygons));
    }
  });

  std::cout << "QuadTree query time: " << qtQueryTime << " ms\n";
  std::cout << "RTree query time: " << rtQueryTime << " ms\n";
  std::cout << "Brute Force query time: " << bfQueryTime << " ms\n";

  int miss_match_qt = 0;
  int good_match_qt = 0;
  int good_match_rt = 0;
  int miss_match_rt = 0;
  // Compare results
  for (auto point : queryPoints) {
    auto polygon_qt = qt.getPointerPolygon(point);
    auto polygon_rt = rt.getPointerPolygon(point);
    auto polygon_bf = bruteForceQuery(point, polygons);

    if (polygon_qt == polygon_bf) {
      ++good_match_qt;
    } else {
      ++miss_match_rt;
    }

    if (polygon_rt == polygon_bf) {
      ++good_match_rt;
    } else {
      ++miss_match_rt;
    }
  }

  std::cout << "Mismatches qt: " << miss_match_qt << "\n"
            << "Good matches: " << good_match_qt << "\n";
  std::cout << "Mismatches rt: " << miss_match_rt << "\n"
            << "Good matches: " << good_match_rt << "\n";

      // Create an image to visualize the field
    cv::Mat image(WORLD_SIZE, WORLD_SIZE, CV_8UC3, cv::Scalar(255, 255, 255));

    // Draw polygons
    for (const auto& polygon : polygons) {
        std::vector<std::vector<cv::Point>> contours = {polygon};
        cv::drawContours(image, contours, 0, getColorFromNumbers(contours[0][0].x, contours[0][0].y ), 2);
    }

    // Draw query points
    for (const auto& point : queryPoints) {
        cv::circle(image, point, 2, cv::Scalar(0, 0, 255), -1);
    }

    // Resize the image for better visibility (optional)
    cv::Mat resized_image;
    cv::resize(image, resized_image, cv::Size(), 0.5, 0.5);

    // Display the image
    cv::imshow("Field with Polygons and Query Points", resized_image);
    cv::waitKey(0);

    // Save the image
    cv::imwrite("field_visualization.png", resized_image);

    // ... (keep your existing code for performance measurements and result comparison)

    return 0;

  return 0;
}
