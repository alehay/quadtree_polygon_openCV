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
  const int NUM_POLYGONS = 1 << 16;
  const int NUM_QUERIES = 1 << 16;
  const int WORLD_SIZE = 1 << 12;

  std::vector<std::vector<cv::Point2i>> polygons;
  for (int i = 0; i < NUM_POLYGONS; ++i) {
    polygons.push_back(generateRandomPolygon(0, WORLD_SIZE, 0, WORLD_SIZE, 4));
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

  return 0;
}
