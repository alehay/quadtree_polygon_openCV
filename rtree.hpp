#ifndef __RTREE_HPP__
#define __RTREE_HPP__

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>

#include <memory>
#include <vector>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

template <typename T, typename P> class RTree {
public:
  using Point = bg::model::point<T, 2, bg::cs::cartesian>;
  using Box = bg::model::box<Point>;
  using Polygon = bg::model::polygon<Point>;
  using Value = std::pair<Box, P>;

private:
  bgi::rtree<Value, bgi::quadratic<16>> rtree;

public:
  RTree() = default;

  bool insert(P polygon) {
    Polygon boostPolygon;
    for (const auto &point : *polygon) {
      bg::append(boostPolygon, Point(point.x, point.y));
    }

    Box bounds;
    bg::envelope(boostPolygon, bounds);

    rtree.insert(std::make_pair(bounds, polygon));
    return true;
  }

  auto getPointerPolygon(const cv::Point_<T> &p) {
    using ReturnType = std::conditional_t<std::is_pointer_v<P>, P, P *>;
    Point queryPoint(p.x, p.y);

    std::vector<Value> result;
    rtree.query(bgi::contains(queryPoint), std::back_inserter(result));

    for (const auto &item : result) {
      if (cv::pointPolygonTest(*item.second, p, false) >= 0) {
        if constexpr (std::is_pointer_v<P>) {
          return item.second;
        } else {
          return &item.second;
        }
      }
    }

    return ReturnType{nullptr};
  }

  std::vector<P> query(const cv::Point_<T> &p) {
    Point queryPoint(p.x, p.y);

    std::vector<Value> result;
    rtree.query(bgi::contains(queryPoint), std::back_inserter(result));

    std::vector<P> polygons;
    for (const auto &item : result) {
      if (cv::pointPolygonTest(*item.second, p, false) >= 0) {
        polygons.push_back(item.second);
      }
    }

    return polygons;
  }
};

#endif // __RTREE_HPP__
