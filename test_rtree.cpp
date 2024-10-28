#include "rtree.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <opencv2/core.hpp>

// Define a simple polygon type for testing
using Polygon = std::shared_ptr<std::vector<cv::Point2f>>;

// Test fixture for RTree
class RTreeTest : public ::testing::Test {
protected:
  RTree<float, Polygon> rtree;

  void SetUp() override {
    // No need to set up the root for RTree
  }
};

// Test inserting a polygon into the RTree
TEST_F(RTreeTest, InsertPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  ASSERT_TRUE(rtree.insert(polygon));
}

// Test querying a point inside a polygon
TEST_F(RTreeTest, QueryPointInsidePolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  rtree.insert(polygon);
  cv::Point2f point(15, 15);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result[0], polygon);
}

// Test querying a point outside any polygon
TEST_F(RTreeTest, QueryPointOutsidePolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  rtree.insert(polygon);
  cv::Point2f point(30, 30);
  auto result = rtree.query(point);

  ASSERT_TRUE(result.empty());
}

// Test inserting multiple polygons into the RTree
TEST_F(RTreeTest, InsertMultiplePolygons) {
  Polygon polygon1 = std::make_shared<std::vector<cv::Point2f>>();
  polygon1->emplace_back(10, 10);
  polygon1->emplace_back(20, 10);
  polygon1->emplace_back(20, 20);
  polygon1->emplace_back(10, 20);

  Polygon polygon2 = std::make_shared<std::vector<cv::Point2f>>();
  polygon2->emplace_back(30, 30);
  polygon2->emplace_back(40, 30);
  polygon2->emplace_back(40, 40);
  polygon2->emplace_back(30, 40);

  ASSERT_TRUE(rtree.insert(polygon1));
  ASSERT_TRUE(rtree.insert(polygon2));
}

// Test querying a point at the boundary of a polygon
TEST_F(RTreeTest, QueryPointAtBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(0, 0);
  polygon->emplace_back(10, 0);
  polygon->emplace_back(10, 10);
  polygon->emplace_back(0, 10);

  rtree.insert(polygon);
  cv::Point2f point(0, 0);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result[0], polygon);
}

// Test inserting a polygon with negative coordinates
TEST_F(RTreeTest, InsertPolygonWithNegativeCoordinates) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(-10, -10);
  polygon->emplace_back(-20, -10);
  polygon->emplace_back(-20, -20);
  polygon->emplace_back(-10, -20);

  ASSERT_TRUE(rtree.insert(polygon));
}

// Test querying a point on the edge of a polygon
TEST_F(RTreeTest, QueryPointOnEdgeOfPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  rtree.insert(polygon);
  cv::Point2f point(20, 15);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result[0], polygon);
}

// Test querying a point at the exact corner of a polygon
TEST_F(RTreeTest, QueryPointAtCornerOfPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  rtree.insert(polygon);
  cv::Point2f point(10, 10);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result[0], polygon);
}

// Test inserting a large number of small polygons
TEST_F(RTreeTest, InsertManySmallPolygons) {
  for (int i = 0; i < 100; ++i) {
    Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
    polygon->emplace_back(i, i);
    polygon->emplace_back(i + 1, i);
    polygon->emplace_back(i + 1, i + 1);
    polygon->emplace_back(i, i + 1);

    ASSERT_TRUE(rtree.insert(polygon));
  }

  cv::Point2f point(50, 50);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
}

// Test inserting and querying multiple polygons in different areas
TEST_F(RTreeTest, InsertAndQueryMultiplePolygonsInDifferentAreas) {
  Polygon polygon1 = std::make_shared<std::vector<cv::Point2f>>();
  polygon1->emplace_back(10, 10);
  polygon1->emplace_back(20, 10);
  polygon1->emplace_back(20, 20);
  polygon1->emplace_back(10, 20);

  Polygon polygon2 = std::make_shared<std::vector<cv::Point2f>>();
  polygon2->emplace_back(70, 70);
  polygon2->emplace_back(80, 70);
  polygon2->emplace_back(80, 80);
  polygon2->emplace_back(70, 80);

  Polygon polygon3 = std::make_shared<std::vector<cv::Point2f>>();
  polygon3->emplace_back(40, 40);
  polygon3->emplace_back(50, 40);
  polygon3->emplace_back(50, 50);
  polygon3->emplace_back(40, 50);

  ASSERT_TRUE(rtree.insert(polygon1));
  ASSERT_TRUE(rtree.insert(polygon2));
  ASSERT_TRUE(rtree.insert(polygon3));

  cv::Point2f point1(15, 15);
  auto result1 = rtree.query(point1);
  ASSERT_EQ(result1.size(), 1);
  ASSERT_EQ(result1[0], polygon1);

  cv::Point2f point2(75, 75);
  auto result2 = rtree.query(point2);
  ASSERT_EQ(result2.size(), 1);
  ASSERT_EQ(result2[0], polygon2);

  cv::Point2f point3(45, 45);
  auto result3 = rtree.query(point3);
  ASSERT_EQ(result3.size(), 1);
  ASSERT_EQ(result3[0], polygon3);
}

// Test inserting a polygon with very large coordinates
TEST_F(RTreeTest, InsertPolygonWithLargeCoordinates) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(1000, 1000);
  polygon->emplace_back(2000, 1000);
  polygon->emplace_back(2000, 2000);
  polygon->emplace_back(1000, 2000);

  ASSERT_TRUE(rtree.insert(polygon));
}

// Test inserting a polygon with collinear points
TEST_F(RTreeTest, InsertPolygonWithCollinearPoints) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(30, 10);
  polygon->emplace_back(40, 10);

  ASSERT_TRUE(rtree.insert(polygon));

  cv::Point2f point(25, 10);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result[0], polygon);
}

// Test inserting a polygon with duplicate points
TEST_F(RTreeTest, InsertPolygonWithDuplicatePoints) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(20, 20); // Duplicate point
  polygon->emplace_back(10, 10); // Duplicate point

  ASSERT_TRUE(rtree.insert(polygon));

  cv::Point2f point(15, 15);
  auto result = rtree.query(point);

  ASSERT_EQ(result.size(), 1);
  ASSERT_EQ(result[0], polygon);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
