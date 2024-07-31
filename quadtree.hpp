#ifndef __QUADTREE_HPP__
#define __QUADTREE_HPP__

#include <memory>
#include <opencv2/core/types.hpp>

#include <cstdint>
#include <memory>
#include <opencv2/core.hpp>
#include <type_traits>
#include <vector>

#include <iostream>

enum class Quadrant : std::uint8_t { TL = 0, TR = 1, BL = 2, BR = 3, NONE = 4 };

template <typename T> class Node {
public:
  cv::Point_<T> TL;
  cv::Point_<T> BR;

  Node(const cv::Point_<T> &_TL, const cv::Point_<T> &_BR) : TL(_TL), BR(_BR) {}

  virtual ~Node() = default; // Virtual destructor for polymorphic base class
  cv::Point_<T> get_median(const cv::Point_<T> &p1,
                           const cv::Point_<T> &p2) const {
    return (p1 + p2) / 2;
  }
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

  //  virtual std::unique_ptr<Node<T>> createConstrainNode() = 0;
  //  virtual std::unique_ptr<Node<T>> createContainNode() = 0;
};

template <typename T> class ConstrainNode : public Node<T> {
public:
  std::unique_ptr<Node<T>> TLNode = nullptr;
  std::unique_ptr<Node<T>> TRNode = nullptr;
  std::unique_ptr<Node<T>> BLNode = nullptr;
  std::unique_ptr<Node<T>> BRNode = nullptr;

  ConstrainNode(cv::Point_<T> _TL, cv::Point_<T> _BR) : Node<T>(_TL, _BR) {}

  ~ConstrainNode() override = default;

  std::unique_ptr<Node<T>> createConstrainNode(Quadrant q) {
    // Implementation for creating a constrain node
  }

  std::unique_ptr<Node<T>> createContainNode() {
    // Implementation for creating a contain node
  }
};

template <typename T, typename Cont> class ContainNode : public Node<T> {
public:
  Cont ContValue;

  // ContainNode(const cv::Point_<T>& _TL, const cv::Point_<T>& _BR, Cont&&
  // _Cont)
  //         : Node<T>(_TL, _BR), ContValue(std::forward<Cont>(_Cont)) {}

  // ContainNode(const cv::Point_<T>& _TL, const cv::Point_<T>& _BR, Cont _Cont)
  //         : Node<T>(_TL, _BR), ContValue(_Cont) {}

  ContainNode(const cv::Point_<T> &_TL, const cv::Point_<T> &_BR, Cont _Cont)
      : Node<T>(_TL, _BR), ContValue(_Cont) {}

  ~ContainNode() override = default;

  std::unique_ptr<Node<T>> createConstrainNode() {
    // Implementation for creating a constrain node
  }

  std::unique_ptr<Node<T>> createContainNode() {
    // Implementation for creating a contain node
  }
};

template <typename T, typename P> class QuadTree {
private:
  std::unique_ptr<ConstrainNode<T>> root = nullptr;
  std::size_t depth = 2;

public:
  QuadTree() = default;

  bool setRoot(cv::Point_<T> TL, cv::Point_<T> BR) {
    root = std::make_unique<ConstrainNode<T>>(TL, BR);
    return true;
  }

  ConstrainNode<T> *insertConstrain(Quadrant q, ConstrainNode<T> *node) {
    std::pair<cv::Point_<T>, cv::Point_<T>> new_rect =
        node->getPointToQuadrant(q);
    switch (q) {
    case Quadrant::TL:
      node->TLNode = std::make_unique<ConstrainNode<T>>(
          ConstrainNode<T>(new_rect.first, new_rect.second));
      return TLNode.get();
    case Quadrant::TR:
      node->TRNode = std::make_unique<ConstrainNode<T>>(
          ConstrainNode<T>(new_rect.first, new_rect.second));
      return TRNode.get();
    case Quadrant::BL:
      node->BLNode = std::make_unique<ConstrainNode<T>>(
          ConstrainNode<T>(new_rect.first, new_rect.second));
      return BLNode.get();
    case Quadrant::BR:
      node->BRNode = std::make_unique<ConstrainNode<T>>(
          ConstrainNode<T>(new_rect.first, new_rect.second));
      return BRNode.get();
    case Quadrant::NONE:
    default:
      return nullptr;
    }
  }

  bool insertContain(Quadrant q, ConstrainNode<T> *node) {
    std::pairr<cv::Point_<T>, cv::Point_<T>> new_rect =
        node->getPointToQuadrant(q);
    switch (q) {
    case Quadrant::TL:
      node->TLNode = std::make_unique<ContainNode<T, P>>(
          ContainNode<T, P>(new_rect.first, new_rect.second));
      return TLNode.get();
    case Quadrant::TR:
      node->TRNode = std::make_unique<ContainNode<T, P>>(
          ContainNode<T, P>(new_rect.first, new_rect.second));
      return TRNode.get();
    case Quadrant::BL:
      node->BLNode = std::make_unique<ContainNode<T, P>>(
          ContainNode<T, P>(new_rect.first, new_rect.second));
      return BLNode.get();
    case Quadrant::BR:
      node->BRNode = std::make_unique<ContainNode<T, P>>(
          ContainNode<T, P>(new_rect.first, new_rect.second));
      return BRNode.get();
    case Quadrant::NONE:
    default:
      return nullptr;
    }
  }

  bool insert(P polygon) {
    if (!root)
      return false;

    int insert_deph = depth;
    ConstrainNode<T> *node = root.get();

    for (int i = 0; i < depth; ++i) {
      std::vector<Quadrant> quadrants;

      std::transform(
          polygon->begin(), polygon->end(), std::back_inserter(quadrants),
          [&](const cv::Point_<T> &p) { return node->getQuadrant(p); });

      bool allInSameQuadrant =
          std::adjacent_find(quadrants.begin(), quadrants.end(),
                             std::not_equal_to<>()) == quadrants.end();

      if (allInSameQuadrant) {
        node =  node->createConstrainNode(quadrants[0]);
      } else () 

    }
    return true;
  }
};

#endif //__QQUADTREE_HPP__