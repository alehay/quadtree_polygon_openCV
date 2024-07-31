#ifndef __QUADTREE_HPP__
#define __QUADTREE_HPP__

#include <memory>
#include <opencv2/core/types.hpp>

#include <cstdint>
#include <memory>
#include <opencv2/core.hpp>
#include <type_traits>
#include <vector>

template <typename T> class constrainNode;
template <typename T, typename Cont> class containNode;

enum Quadrant : std::uint8_t { TL = 0, TR = 1, BL = 2, BR = 3, NONE = 4 };

template <typename T> class Node {
public:
  cv::Point<T> TL;
  cv::Point<T> TR;
  cv::Point<T> BR;
  cv::Point<T> BL;

  Quadrant get_quadrant_point(Point<T> &p) {
    if (p.x < TL.x or p.y < TL.y) {
      return Quadrant::NONE;
    }
    if (p.x > BR.x or p.y > BR.y) {
      return Quadrant::NONE;
    }
    cv::Point<T> median = (TL + TR + BL + BR) / 4;
    if (p.x >= TL.x and p.y >= TL.y) {
      if (p.x < median.x and p.y < median.y) {
        return Quadrant::TL;
      }
      if (p.x >= median.x and p.y < median.y) {
        return Quadrant::TR;
      }
      if (p.x < median.x and p.y >= median.y) {
        return Quadrant::BL;
      }
      if (p.x >= median.x and p.y >= median.y) {
        return Quadrant::BR;
      }
    }
    return Quadrant::NONE;
  }

  Node(cv::Point<T> _TL, cv::Point<T> _TR, cv::Point<T> _BR, cv::Point<T> _BL)
      : TL(_TL), TR(_TR), BR(_BR), BL(_BL) {}

  virtual constrainNode<T> create_constrain_node();
  virtual containNode<T, Cont> create_contain_node();

  virtual ~Node() = default; // Virtual destructor for polymorphic base class
};

template <typename T> class constrainNode : public Node<T> {
public:
  std::unique_ptr < Node<T> TLNode = nullptr;
  std::unique_ptr < Node<T> TRNode = nullptr;
  std::unique_ptr < Node<T> BLNode = nullptr;
  std::unique_ptr < Node<T> BRNode = nullptr;

  constrainNode(cv::Point<T> _TL, cv::Point<T> _TR, cv::Point<T> _BR,
                cv::Point<T> _BL)
      : Node<T>(_TL, _TR, _BR, _BL) {}

  ~constrainNode() override = default;
};

template <typename T, typename Cont> class containNode : public Node<T> {
public:
  Cont ContValue;

  containNode(cv::Point<T> _TL, cv::Point<T> _TR, cv::Point<T> _BR,
              cv::Point<T> _BL, Cont &&_Cont)
      : Node<T, Cont>(_TL, _TR, _BR, _BL),
        ContValue(std::forward<Cont>(_Cont)) {}

  ~containNode() override {
    if constexpr (!std::is_pointer<Cont>::value) {
    }
  }
};

template <typename T, typename P> class QuadTree {
private:
  std::unique_ptr < constrainNode<T> root = nullptr;
  std::size_t depth = 3;

public:
  QuadTree() = default;

  bool set_root(cv::Point<T> TL, cv::Point<T> TR, cv::Point<T> BR,
                cv::Point<T> BL) {
    root = std::make_unique < constrainNode<T>(TL, TR, BR, BL);
    return true;
  }

  bool insert(const std::vector<cv::Point<T>> &polygon) {
    if (root == nullptr) {
      return false;
    }

    Node<T> *node = root.get();

    std::vector<Quadrant> quadrant;

    std::transform(polygon.begin(), polygon.end(), std::back_inserter(quadrant),
                   [&](const Point &p) { return root->get_quadrant_point(p); });

    bool all_in_quadrant = std::equal(vec.begin() + 1, vec.end(), vec.begin());

    if (all_in_quadrant) {

      Point<T> median = (TL + TR + BL + BR) / 4;

      switch (quadrant[0]) {
      case Quadrant::TL break; 
        node->TLNode = std::make_unique < containNode<T, cv::Point<T>>(TL, (TL.x, median.y), 
      case Quadrant::TR:
        node->TRNode = std::make_unique < containNode<T, cv::Point<T>>() break;
      case Quadrant::BL:
        break;
      case Quadrant::BR:
        break;
      case Quadrant::NONE:
      default:
        break;
      }
    }
  }
};

#endif __QQUADTREE_HPP__