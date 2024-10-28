#ifndef __QUADTREE_HPP__
#define __QUADTREE_HPP__

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core.hpp>

#include <cstdint>
#include <memory>
#include <type_traits>
#include <vector>

#include <iostream>

enum class Quadrant : std::uint8_t { TL = 0, TR = 1, BL = 2, BR = 3, NONE = 4 };

template <typename T, typename P> class Node {
public:
  cv::Point_<T> TL;
  cv::Point_<T> BR;
  std::unique_ptr<Node<T, P>> TLNode = nullptr;
  std::unique_ptr<Node<T, P>> TRNode = nullptr;
  std::unique_ptr<Node<T, P>> BLNode = nullptr;
  std::unique_ptr<Node<T, P>> BRNode = nullptr;
  std::vector<P> contain;

public:
  Node(const cv::Point_<T> &_TL, const cv::Point_<T> &_BR) : TL(_TL), BR(_BR) {}

  void add(P p) { contain.push_back(std::forward<P>(p)); }

  bool empty() const { return contain.empty(); }

  cv::Point_<T> get_median(const cv::Point_<T> &p1,
                           const cv::Point_<T> &p2) const {
    return (p1 + p2) / 2;
  }

  cv::Point_<T> getTL() const { return TL; }
  cv::Point_<T> getBR() const { return BR; }

  Quadrant getQuadrant(const cv::Point_<T> &p) const {
    if (p.x < TL.x || p.y < TL.y || p.x > BR.x || p.y > BR.y) {
      return Quadrant::NONE;
    }
    cv::Point_<T> median = get_median(TL, BR);

    if (p.x < median.x && p.y < median.y)
      return Quadrant::TL;
    if (p.x >= median.x && p.y < median.y)
      return Quadrant::TR;
    if (p.x < median.x && p.y >= median.y)
      return Quadrant::BL;
    if (p.x >= median.x && p.y >= median.y)
      return Quadrant::BR;

    return Quadrant::NONE;
  }

  Node<T, P> *getNodeQuadrant(Quadrant q) {
    switch (q) {
    case Quadrant::TL:
      return TLNode.get();
    case Quadrant::TR:
      return TRNode.get();
    case Quadrant::BL:
      return BLNode.get();
    case Quadrant::BR:
      return BRNode.get();
    case Quadrant::NONE:
    default:
      return nullptr;
    }
  }

  std::pair<cv::Point_<T>, cv::Point_<T>> getPointToQuadrant(Quadrant q) {
    cv::Point_<T> MED = get_median(TL, BR);
    switch (q) {
    case Quadrant::TL:
      return std::make_pair(TL, MED);
    case Quadrant::TR:
      return std::make_pair(cv::Point_<T>(MED.x, TL.y),
                            cv::Point_<T>(BR.x, MED.y));
    case Quadrant::BL:
      return std::make_pair(cv::Point_<T>(TL.x, MED.y),
                            cv::Point_<T>(MED.x, BR.y));
    case Quadrant::BR:
      return std::make_pair(MED, BR);

    case Quadrant::NONE:
    default:
      return std::make_pair(cv::Point_<T>(), cv::Point_<T>());
    }
  }
};

template <typename T, typename P> class QuadTree {
protected:
  std::unique_ptr<Node<T, P>> root = nullptr;
  std::size_t depth = 6;

private:
  auto getContainPolygon(Node<T, P> *node, const cv::Point_<T> &p) {
    using ReturnType = std::conditional_t<std::is_pointer_v<P>, P, P *>;
    for (auto &pol : node->contain) {
      double result = cv::pointPolygonTest((*pol), p, false);
      if (result >= 0) {
        if constexpr (std::is_pointer_v<P>) {
          return pol;
        } else {
          return &pol;
        }
      }
    }
    return ReturnType{nullptr}; // Ensure nullptr is cast to ReturnType
  }

public:
  QuadTree() = default;

  bool setRoot(cv::Point_<T> TL, cv::Point_<T> BR) {
    root = std::make_unique<Node<T, P>>(TL, BR);
    return true;
  }

  Node<T, P> *insertConstrain(Quadrant q, Node<T, P> *node) {
    std::pair<cv::Point_<T>, cv::Point_<T>> new_rect =
        node->getPointToQuadrant(q);
    switch (q) {
    case Quadrant::TL:
      if (node->TLNode == nullptr) {
        node->TLNode = std::make_unique<Node<T, P>>(
            Node<T, P>(new_rect.first, new_rect.second));
      }
      return node->TLNode.get();
    case Quadrant::TR:
      if (node->TRNode == nullptr) {
        node->TRNode = std::make_unique<Node<T, P>>(
            Node<T, P>(new_rect.first, new_rect.second));
      }
      return node->TRNode.get();
    case Quadrant::BL:
      if (node->BRNode == nullptr) {
        node->BLNode = std::make_unique<Node<T, P>>(
            Node<T, P>(new_rect.first, new_rect.second));
      }
      return node->BLNode.get();
    case Quadrant::BR:
      if (node->BRNode == nullptr) {
        node->BRNode = std::make_unique<Node<T, P>>(
            Node<T, P>(new_rect.first, new_rect.second));
      }
      return node->BRNode.get();
    case Quadrant::NONE:
    default:
      return nullptr;
    }
  }

  bool insert(P polygon) {
    if (root == nullptr) {
      return false;
    }

    Node<T, P> *node = root.get();

    for (int i = 0; i < depth; ++i) {
      std::vector<Quadrant> quadrants;

      std::transform(
          polygon->begin(), polygon->end(), std::back_inserter(quadrants),
          [&](const cv::Point_<T> &p) { return node->getQuadrant(p); });

      // Check if all points of the polygon fall into the same quadrant
      bool allInSameQuadrant =
          std::adjacent_find(quadrants.begin(), quadrants.end(),
                             std::not_equal_to<>()) == quadrants.end();

      bool noneDetect = std::find(quadrants.begin(), quadrants.end(),
                                  Quadrant::NONE) != quadrants.end();

      if (noneDetect) {
        return false;
      }

      if (allInSameQuadrant) {
        // all points of the polygon fall into one of the quadrants.
        // hence the polygon and all points that belong to it are inside the
        // quadrant.
        node = insertConstrain(quadrants[0], node);

      } else {
        node->add(polygon);
        return true;
      }
      if (node == nullptr) {
        return false;
      }
    }
    node->add(polygon);
    return true;
  }

  auto getPointerPolygon(cv::Point_<T> const &p) {
    using ReturnType = std::conditional_t<std::is_pointer_v<P>, P, P *>;
    if (root == nullptr) {
      return ReturnType{nullptr}; // Ensure nullptr is cast to ReturnType
    }

    Node<T, P> *node = root.get();

    for (int i = 0; i < depth; ++i) {
      if (not node->empty()) {

        ReturnType pol = getContainPolygon(node, p);
        if (pol != nullptr) {
          return pol;
        }
      }
      Quadrant q = node->getQuadrant(p);
      if (q == Quadrant::NONE) {
        return ReturnType{nullptr}; // Ensure nullptr is cast to ReturnType
      }
      node = node->getNodeQuadrant(q);
      if (node == nullptr) {
        return ReturnType{nullptr}; // Ensure nullptr is cast to ReturnType
      }
    }
    return ReturnType{nullptr}; // Ensure nullptr is cast to ReturnType
  }

  const std::vector<P> *query(cv::Point_<T> &p) {
    if (root == nullptr) {
      return nullptr;
    }

    Node<T, P> *node = root.get();

    for (int i = 0; i < depth; ++i) {
      if (not node->empty()) {
        // there is a possibility that the point hit the polygon at this level
        for (auto &pol : node->contain) {
          double result = cv::pointPolygonTest((*pol), p, false);
          if (result >= 0) {
            return &node->contain;
          }
        }
      }
      Quadrant q = node->getQuadrant(p);
      if (q == Quadrant::NONE) {
        return nullptr;
      }
      node = node->getNodeQuadrant(q);
      if (node == nullptr) {
        return nullptr;
      }
    }
    if (node != nullptr) {
      return &node->contain;
    }
    return nullptr;
  }
};

#endif //__QQUADTREE_HPP__