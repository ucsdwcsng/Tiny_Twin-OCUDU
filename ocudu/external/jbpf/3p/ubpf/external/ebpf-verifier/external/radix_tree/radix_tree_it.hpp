#pragma once

#include <functional>

// forward declaration
template <typename K, typename T, class Compare = std::less<K>>
class radix_tree;
template <typename K, typename T, class Compare = std::less<K>>
class radix_tree_node;

template <typename K, typename T, class Compare = std::less<K>>
class radix_tree_it {
    friend class radix_tree<K, T, Compare>;

  public:
    // Iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<std::pair<const K, T>, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference = value_type&;

    radix_tree_it() : m_pointee(nullptr) {}
    radix_tree_it(const radix_tree_it& r) : m_pointee(r.m_pointee) {}
    radix_tree_it& operator=(const radix_tree_it& r) = default;
    ~radix_tree_it() = default;

    std::pair<const K, T>& operator*() const;
    std::pair<const K, T>* operator->() const;
    const radix_tree_it& operator++();
    radix_tree_it operator++(int);

    // const radix_tree_it& operator-- ();
    bool operator!=(const radix_tree_it& lhs) const;
    bool operator==(const radix_tree_it& lhs) const;

  private:
    radix_tree_node<K, T, Compare>* m_pointee;
    explicit radix_tree_it(radix_tree_node<K, T, Compare>* p) : m_pointee(p) {}

    radix_tree_node<K, T, Compare>* increment(radix_tree_node<K, T, Compare>* node) const;
    radix_tree_node<K, T, Compare>* descend(radix_tree_node<K, T, Compare>* node) const;
};

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree_it<K, T, Compare>::increment(radix_tree_node<K, T, Compare>* node) const {
    radix_tree_node<K, T, Compare>* parent = node->m_parent;

    if (parent == nullptr) {
        return nullptr;
    }

    typename radix_tree_node<K, T, Compare>::it_child it = parent->m_children.find(node->m_key);
    assert(it != parent->m_children.end());
    ++it;

    if (it == parent->m_children.end()) {
        return increment(parent);
    }
    return descend(it->second);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree_it<K, T, Compare>::descend(radix_tree_node<K, T, Compare>* node) const {
    if (node->m_is_leaf) {
        return node;
    }

    typename radix_tree_node<K, T, Compare>::it_child it = node->m_children.begin();

    assert(it != node->m_children.end());

    return descend(it->second);
}

template <typename K, typename T, typename Compare>
std::pair<const K, T>& radix_tree_it<K, T, Compare>::operator*() const {
    return *m_pointee->m_value;
}

template <typename K, typename T, typename Compare>
std::pair<const K, T>* radix_tree_it<K, T, Compare>::operator->() const {
    return m_pointee->m_value;
}

template <typename K, typename T, typename Compare>
bool radix_tree_it<K, T, Compare>::operator!=(const radix_tree_it& lhs) const {
    return m_pointee != lhs.m_pointee;
}

template <typename K, typename T, typename Compare>
bool radix_tree_it<K, T, Compare>::operator==(const radix_tree_it& lhs) const {
    return m_pointee == lhs.m_pointee;
}

template <typename K, typename T, typename Compare>
const radix_tree_it<K, T, Compare>& radix_tree_it<K, T, Compare>::operator++() {
    if (m_pointee != nullptr) { // it is undefined behaviour to dereference iterator that is out of bounds...
        m_pointee = increment(m_pointee);
    }
    return *this;
}

template <typename K, typename T, typename Compare>
radix_tree_it<K, T, Compare> radix_tree_it<K, T, Compare>::operator++(int) {
    radix_tree_it copy(*this);
    ++(*this);
    return copy;
}
