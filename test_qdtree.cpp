#include "quadtree.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <opencv2/core.hpp>

// Define a simple polygon type for testing
using Polygon = std::shared_ptr<std::vector<cv::Point2f>>;

// Test fixture for QuadTree
class QuadTreeTest : public ::testing::Test {
protected:
  QuadTree<int, Polygon> quadtree;

  void SetUp() override {
    // Set up the root of the QuadTree
    quadtree.setRoot(cv::Point_<int>(0, 0), cv::Point_<int>(100, 100));
  }
};

// Test setting the root of the QuadTree
TEST_F(QuadTreeTest, SetRoot) {
  ASSERT_TRUE(
      quadtree.setRoot(cv::Point_<int>(0, 0), cv::Point_<int>(100, 100)));
}

// Test querying a point within a polygon
TEST_F(QuadTreeTest, QueryPointInPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);

  cv::Point_<int> point(15, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
}

// Test querying a point outside any polygon
TEST_F(QuadTreeTest, QueryPointOutsidePolygon_2) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);

  cv::Point_<int> point(30, 30);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_EQ(result, nullptr);
}

// Test inserting multiple polygons and querying a point
TEST_F(QuadTreeTest, InsertMultiplePolygons) {
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

  quadtree.insert(polygon1);
  quadtree.insert(polygon2);

  cv::Point_<int> point(35, 35);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
}

// Test querying a point on the boundary of a polygon
TEST_F(QuadTreeTest, QueryPointOnBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);

  cv::Point_<int> point(10, 10);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result,
            nullptr); // Assuming pointPolygonTest returns 0 for boundary points
}
// Test inserting a polygon into the QuadTree
TEST_F(QuadTreeTest, InsertPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  ASSERT_TRUE(quadtree.insert(polygon));
}

// Test querying a point inside a polygon
TEST_F(QuadTreeTest, QueryPointInsidePolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);
  cv::Point_<int> point(15, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test querying a point outside any polygon
TEST_F(QuadTreeTest, QueryPointOutsidePolygon_3) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);
  cv::Point_<int> point(30, 30);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_EQ(result, nullptr);
}

// Test inserting multiple polygons into the QuadTree
TEST_F(QuadTreeTest, InsertMultiplePolygons_2) {
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

  ASSERT_TRUE(quadtree.insert(polygon1));
  ASSERT_TRUE(quadtree.insert(polygon2));
}

// Test querying a point at the boundary of the QuadTree
TEST_F(QuadTreeTest, QueryPointAtBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(0, 0);
  polygon->emplace_back(10, 0);
  polygon->emplace_back(10, 10);
  polygon->emplace_back(0, 10);

  quadtree.insert(polygon);
  cv::Point_<int> point(0, 0);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting a polygon that spans multiple quadrants
TEST_F(QuadTreeTest, InsertPolygonSpanningMultipleQuadrants) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(90, 10);
  polygon->emplace_back(90, 90);
  polygon->emplace_back(10, 90);

  ASSERT_TRUE(quadtree.insert(polygon));
}

// Test querying a point in an empty QuadTree
TEST_F(QuadTreeTest, QueryPointInEmptyQuadTree) {
  cv::Point_<int> point(50, 50);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_EQ(result, nullptr);
}

// Test inserting and querying a complex polygon
TEST_F(QuadTreeTest, InsertAndQueryComplexPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(15, 25);
  polygon->emplace_back(10, 20);

  ASSERT_TRUE(quadtree.insert(polygon));

  cv::Point_<int> point(15, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting a polygon with negative coordinates
TEST_F(QuadTreeTest, InsertPolygonWithNegativeCoordinates) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(-10, -10);
  polygon->emplace_back(-20, -10);
  polygon->emplace_back(-20, -20);
  polygon->emplace_back(-10, -20);

  ASSERT_FALSE(quadtree.insert(polygon));
}

// Test inserting and querying a polygon at the root boundary
TEST_F(QuadTreeTest, InsertAndQueryPolygonAtRootBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(0, 0);
  polygon->emplace_back(100, 0);
  polygon->emplace_back(100, 100);
  polygon->emplace_back(0, 100);

  ASSERT_TRUE(quadtree.insert(polygon));

  cv::Point_<int> point(50, 50);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}
// Test querying a point on the edge of a polygon
TEST_F(QuadTreeTest, QueryPointOnEdgeOfPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);
  cv::Point_<int> point(20, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test querying a point at the exact corner of a polygon
TEST_F(QuadTreeTest, QueryPointAtCornerOfPolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);
  cv::Point_<int> point(10, 10);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting a polygon with points outside the QuadTree boundary
TEST_F(QuadTreeTest, InsertPolygonWithPointsOutsideBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(110, 10);  // Point outside boundary
  polygon->emplace_back(110, 110); // Point outside boundary
  polygon->emplace_back(10, 110);  // Point outside boundary

  ASSERT_FALSE(quadtree.insert(polygon));
}

// Test querying a point that lies exactly on the boundary between quadrants
TEST_F(QuadTreeTest, QueryPointOnQuadrantBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(25, 25);
  polygon->emplace_back(75, 25);
  polygon->emplace_back(75, 75);
  polygon->emplace_back(25, 75);

  quadtree.insert(polygon);
  cv::Point_<int> point(50, 50);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

/*

for my application task at the moment, the polygons do not intersect, but it may
be necessary in the future
// Test inserting a polygon with overlapping coordinates
TEST_F(QuadTreeTest, InsertPolygonWithOverlappingCoordinates) {
  Polygon polygon1 = std::make_shared<std::vector<cv::Point2f>>();
  polygon1->emplace_back(10, 10);
  polygon1->emplace_back(20, 10);
  polygon1->emplace_back(20, 20);
  polygon1->emplace_back(10, 20);

  Polygon polygon2 = std::make_shared<std::vector<cv::Point2f>>();
  polygon2->emplace_back(15, 15);
  polygon2->emplace_back(25, 15);
  polygon2->emplace_back(25, 25);
  polygon2->emplace_back(15, 25);

  ASSERT_TRUE(quadtree.insert(polygon1));
  ASSERT_TRUE(quadtree.insert(polygon2));

  cv::Point_<int> point(15, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 2);
  ASSERT_TRUE(std::find(result->begin(), result->end(), polygon1) !=
              result->end());
  ASSERT_TRUE(std::find(result->begin(), result->end(), polygon2) !=
              result->end());
}
*/
// Test querying a point in a deeply nested quadrant
TEST_F(QuadTreeTest, QueryPointInDeeplyNestedQuadrant) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(1, 1);
  polygon->emplace_back(2, 1);
  polygon->emplace_back(2, 2);
  polygon->emplace_back(1, 2);

  quadtree.insert(polygon);
  cv::Point_<int> point(1, 1);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting a large number of small polygons
TEST_F(QuadTreeTest, InsertManySmallPolygons) {
  for (int i = 0; i < 100; ++i) {
    Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
    polygon->emplace_back(i, i);
    polygon->emplace_back(i + 1, i);
    polygon->emplace_back(i + 1, i + 1);
    polygon->emplace_back(i, i + 1);

    ASSERT_TRUE(quadtree.insert(polygon));
  }

  cv::Point_<int> point(50, 50);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
}

// Test querying a point that lies exactly on the median line
TEST_F(QuadTreeTest, QueryPointOnMedianLine) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(25, 25);
  polygon->emplace_back(75, 25);
  polygon->emplace_back(75, 75);
  polygon->emplace_back(25, 75);

  quadtree.insert(polygon);
  cv::Point_<int> point(50, 50);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting and querying multiple polygons in different quadrants
TEST_F(QuadTreeTest, InsertAndQueryMultiplePolygonsInDifferentQuadrants) {
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

  ASSERT_TRUE(quadtree.insert(polygon1));
  ASSERT_TRUE(quadtree.insert(polygon2));
  ASSERT_TRUE(quadtree.insert(polygon3));

  cv::Point_<int> point1(15, 15);
  const std::vector<Polygon> *result1 = quadtree.query(point1);
  ASSERT_NE(result1, nullptr);
  ASSERT_EQ(result1->size(), 1);
  ASSERT_EQ(result1->at(0), polygon1);

  cv::Point_<int> point2(75, 75);
  const std::vector<Polygon> *result2 = quadtree.query(point2);
  ASSERT_NE(result2, nullptr);
  ASSERT_EQ(result2->size(), 1);
  ASSERT_EQ(result2->at(0), polygon2);

  cv::Point_<int> point3(45, 45);
  const std::vector<Polygon> *result3 = quadtree.query(point3);
  ASSERT_NE(result3, nullptr);
  ASSERT_EQ(result3->size(), 1);
  ASSERT_EQ(result3->at(0), polygon3);
}

// Test querying a point that lies exactly at the root boundary
TEST_F(QuadTreeTest, QueryPointAtRootBoundary) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(0, 0);
  polygon->emplace_back(100, 0);
  polygon->emplace_back(100, 100);
  polygon->emplace_back(0, 100);

  quadtree.insert(polygon);
  cv::Point_<int> point(100, 100);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting a polygon with very large coordinates
TEST_F(QuadTreeTest, InsertPolygonWithLargeCoordinates) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(1000, 1000);
  polygon->emplace_back(2000, 1000);
  polygon->emplace_back(2000, 2000);
  polygon->emplace_back(1000, 2000);

  ASSERT_FALSE(quadtree.insert(polygon));
}

// Test querying a point after multiple insertions and removals
TEST_F(QuadTreeTest, QueryPointAfterMultipleInsertionsAndRemovals) {
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

  quadtree.insert(polygon1);
  quadtree.insert(polygon2);

  // Assuming a remove function exists
  // quadtree.remove(polygon1);

  cv::Point_<int> point(35, 35);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon2);
}

// Test querying a point that lies exactly at the center of the QuadTree
TEST_F(QuadTreeTest, QueryPointAtCenter) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(40, 40);
  polygon->emplace_back(60, 40);
  polygon->emplace_back(60, 60);
  polygon->emplace_back(40, 60);

  quadtree.insert(polygon);
  cv::Point_<int> point(50, 50);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}
/*
// Test inserting polygons with overlapping boundaries
TEST_F(QuadTreeTest, InsertPolygonsWithOverlappingBoundaries) {
    Polygon polygon1 = std::make_shared<std::vector<cv::Point2f>>();
    polygon1->emplace_back(10, 10);
    polygon1->emplace_back(30, 10);
    polygon1->emplace_back(30, 30);
    polygon1->emplace_back(10, 30);

    Polygon polygon2 = std::make_shared<std::vector<cv::Point2f>>();
    polygon2->emplace_back(20, 20);
    polygon2->emplace_back(40, 20);
    polygon2->emplace_back(40, 40);
    polygon2->emplace_back(20, 40);

    ASSERT_TRUE(quadtree.insert(polygon1));
    ASSERT_TRUE(quadtree.insert(polygon2));

    cv::Point_<int> point(25, 25);
    const std::vector<Polygon>* result = quadtree.query(point);

    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->size(), 2);
    ASSERT_TRUE(std::find(result->begin(), result->end(), polygon1) !=
result->end()); ASSERT_TRUE(std::find(result->begin(), result->end(), polygon2)
!= result->end());
}
*/

// Test inserting a polygon with collinear points
TEST_F(QuadTreeTest, InsertPolygonWithCollinearPoints) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(30, 10);
  polygon->emplace_back(40, 10);

  ASSERT_TRUE(quadtree.insert(polygon));

  cv::Point_<int> point(25, 10);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test inserting a polygon with duplicate points
TEST_F(QuadTreeTest, InsertPolygonWithDuplicatePoints) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(20, 20); // Duplicate point
  polygon->emplace_back(10, 10); // Duplicate point

  ASSERT_TRUE(quadtree.insert(polygon));

  cv::Point_<int> point(15, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_NE(result, nullptr);
  ASSERT_EQ(result->size(), 1);
  ASSERT_EQ(result->at(0), polygon);
}

// Test querying a point after inserting a polygon with holes
/*
TEST_F(QuadTreeTest, QueryPointAfterInsertingPolygonWithHoles) {
    Polygon outerPolygon = std::make_shared<std::vector<cv::Point2f>>();
    outerPolygon->emplace_back(10, 10);
    outerPolygon->emplace_back(50, 10);
    outerPolygon->emplace_back(50, 50);
    outerPolygon->emplace_back(10, 50);

    Polygon innerPolygon = std::make_shared<std::vector<cv::Point2f>>();
    innerPolygon->emplace_back(20, 20);
    innerPolygon->emplace_back(40, 20);
    innerPolygon->emplace_back(40, 40);
    innerPolygon->emplace_back(20, 40);

    ASSERT_TRUE(quadtree.insert(outerPolygon));
    ASSERT_TRUE(quadtree.insert(innerPolygon));

    cv::Point_<int> point(30, 30);
    const std::vector<Polygon>* result = quadtree.query(point);

    ASSERT_NE(result, nullptr);
    ASSERT_EQ(result->size(), 2);
    ASSERT_TRUE(std::find(result->begin(), result->end(), outerPolygon) !=
result->end()); ASSERT_TRUE(std::find(result->begin(), result->end(),
innerPolygon) != result->end());
}
*/

/*
for my application task at the moment, the polygons do not intersect, but it may
be necessary in the future

// Test inserting a polygon with zero area
TEST_F(QuadTreeTest, InsertPolygonWithZeroArea) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(10, 10);
  polygon->emplace_back(10, 10);
  polygon->emplace_back(10, 10);

  ASSERT_FALSE(quadtree.insert(polygon));
}
*/

// TO DO this !
// Test querying a point after inserting and removing a polygon
/*
TEST_F(QuadTreeTest, QueryPointAfterInsertAndRemovePolygon) {
  Polygon polygon = std::make_shared<std::vector<cv::Point2f>>();
  polygon->emplace_back(10, 10);
  polygon->emplace_back(20, 10);
  polygon->emplace_back(20, 20);
  polygon->emplace_back(10, 20);

  quadtree.insert(polygon);
  // Assuming a remove function exists
  // quadtree.remove(polygon);

  cv::Point_<int> point(15, 15);
  const std::vector<Polygon> *result = quadtree.query(point);

  ASSERT_EQ(result, nullptr);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
*/

