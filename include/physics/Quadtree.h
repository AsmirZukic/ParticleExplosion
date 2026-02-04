#pragma once
#include "Perun/Math/Vector2.h"
#include <vector>
#include <memory>

struct AABB {
    float x, y, width, height;
    
    bool Contains(const Perun::Math::Vector2& point) const {
        return point.x >= x && point.x < x + width &&
               point.y >= y && point.y < y + height;
    }
    
    bool Intersects(const AABB& other) const {
        return x < other.x + other.width && x + width > other.x &&
               y < other.y + other.height && y + height > other.y;
    }
};

template <typename T>
class Quadtree {
public:
    Quadtree(const AABB& boundary, int capacity)
        : m_Boundary(boundary), m_Capacity(capacity) {}
    
    bool Insert(T* item) {
        if (!m_Boundary.Contains(item->Position)) {
            return false;
        }

        if (m_Items.size() < m_Capacity) {
            m_Items.push_back(item);
            return true;
        }

        if (!m_Divided) {
            Subdivide();
        }

        if (northeast->Insert(item)) return true;
        if (northwest->Insert(item)) return true;
        if (southeast->Insert(item)) return true;
        if (southwest->Insert(item)) return true;

        return false;
    }

    void Query(const AABB& range, std::vector<T*>& found) {
        if (!m_Boundary.Intersects(range)) {
            return;
        }

        for (auto* item : m_Items) {
            if (range.Contains(item->Position)) {
                found.push_back(item);
            }
        }

        if (m_Divided) {
            northeast->Query(range, found);
            northwest->Query(range, found);
            southeast->Query(range, found);
            southwest->Query(range, found);
        }
    }

    void Clear() {
        m_Items.clear();
        northeast.reset();
        northwest.reset();
        southeast.reset();
        southwest.reset();
        m_Divided = false;
    }

private:
    AABB m_Boundary;
    int m_Capacity;
    std::vector<T*> m_Items;
    bool m_Divided = false;
    
    std::unique_ptr<Quadtree<T>> northeast;
    std::unique_ptr<Quadtree<T>> northwest;
    std::unique_ptr<Quadtree<T>> southeast;
    std::unique_ptr<Quadtree<T>> southwest;
    
    void Subdivide() {
        float x = m_Boundary.x;
        float y = m_Boundary.y;
        float w = m_Boundary.width / 2.0f;
        float h = m_Boundary.height / 2.0f;

        northeast = std::make_unique<Quadtree<T>>(AABB{x + w, y, w, h}, m_Capacity);
        northwest = std::make_unique<Quadtree<T>>(AABB{x, y, w, h}, m_Capacity);
        southeast = std::make_unique<Quadtree<T>>(AABB{x + w, y + h, w, h}, m_Capacity);
        southwest = std::make_unique<Quadtree<T>>(AABB{x, y + h, w, h}, m_Capacity);

        m_Divided = true;
    }
};
