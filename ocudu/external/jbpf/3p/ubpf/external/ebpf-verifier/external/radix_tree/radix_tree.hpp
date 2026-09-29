#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "radix_tree_it.hpp"
#include "radix_tree_node.hpp"

template <typename K>
K radix_substr(const K& key, int begin, int num);

template <>
inline std::string radix_substr<std::string>(const std::string& key, const int begin, const int num) {
    return key.substr(begin, num);
}

template <typename K>
K radix_join(const K& key1, const K& key2);

template <>
inline std::string radix_join<std::string>(const std::string& key1, const std::string& key2) {
    return key1 + key2;
}

template <typename K>
int radix_length(const K& key);

template <>
inline int radix_length<std::string>(const std::string& key) {
    return static_cast<int>(key.size());
}

template <typename K, typename T, typename Compare>
class radix_tree {
  public:
    typedef K key_type;
    typedef T mapped_type;
    typedef std::pair<const K, T> value_type;
    typedef radix_tree_it<K, T, Compare> iterator;
    typedef std::size_t size_type;

    radix_tree() = default;

    explicit radix_tree(Compare pred) : m_predicate(pred) {}

    ~radix_tree() = default;

    [[nodiscard]]
    size_type size() const {
        return m_size;
    }

    [[nodiscard]]
    bool empty() const {
        return m_size == 0;
    }

    void clear() {
        m_root.reset();
        m_size = 0;
    }

    iterator find(const K& key);

    iterator begin();

    iterator end();

    std::pair<iterator, bool> insert(const value_type& val);

    bool erase(const K& key);

    void erase(iterator it);

    void prefix_match(const K& key, std::vector<iterator>& vec);

    void greedy_match(const K& key, std::vector<iterator>& vec);

    iterator longest_match(const K& key);

    T& operator[](const K& lhs);

    template <class UnaryPred>
    void remove_if(UnaryPred pred) {
        iterator backIt;
        for (iterator it = begin(); it != end(); it = backIt) {
            backIt = it;
            ++backIt;
            if (K toDelete = it->first; pred(toDelete)) {
                erase(toDelete);
            }
        }
    }

    radix_tree(const radix_tree& other) = delete;

    radix_tree& operator=(radix_tree other) = delete;

  private:
    size_type m_size{};
    std::unique_ptr<radix_tree_node<K, T, Compare>> m_root{};
    radix_tree_node<K, T, Compare>* root() { return m_root.get(); }

    Compare m_predicate{};

    radix_tree_node<K, T, Compare>* begin(radix_tree_node<K, T, Compare>* node);

    radix_tree_node<K, T, Compare>* find_node(const K& key, radix_tree_node<K, T, Compare>* node, int depth);

    radix_tree_node<K, T, Compare>* append(radix_tree_node<K, T, Compare>* parent, const value_type& val);

    radix_tree_node<K, T, Compare>* prepend(radix_tree_node<K, T, Compare>* node, const value_type& val);

    void greedy_match(radix_tree_node<K, T, Compare>* node, std::vector<iterator>& vec);
};

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::prefix_match(const K& key, std::vector<iterator>& vec) {
    vec.clear();

    if (!m_root) {
        return;
    }

    K key_sub1, key_sub2;

    radix_tree_node<K, T, Compare>* node = find_node(key, root(), 0);

    if (node->m_is_leaf) {
        node = node->m_parent;
    }

    int len = radix_length(key) - node->m_depth;
    key_sub1 = radix_substr(key, node->m_depth, len);
    key_sub2 = radix_substr(node->m_key, 0, len);

    if (key_sub1 != key_sub2) {
        return;
    }

    greedy_match(node, vec);
}

template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::longest_match(const K& key) {
    if (!m_root) {
        return iterator(nullptr);
    }

    K key_sub;

    radix_tree_node<K, T, Compare>* node = find_node(key, root(), 0);

    if (node->m_is_leaf) {
        return iterator(node);
    }

    key_sub = radix_substr(key, node->m_depth, radix_length(node->m_key));

    if (!(key_sub == node->m_key)) {
        node = node->m_parent;
    }

    K nul = radix_substr(key, 0, 0);

    while (node != nullptr) {
        typename radix_tree_node<K, T, Compare>::it_child it;
        it = node->m_children.find(nul);
        if (it != node->m_children.end() && it->second->m_is_leaf) {
            return iterator(it->second);
        }

        node = node->m_parent;
    }

    return iterator(nullptr);
}

template <typename K, typename T, typename Compare>
// ReSharper disable once CppMemberFunctionMayBeStatic
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::end() {
    return iterator(nullptr);
}

template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::begin() {
    radix_tree_node<K, T, Compare>* node;

    if (!m_root || m_size == 0) {
        node = nullptr;
    } else {
        node = begin(root());
    }

    return iterator(node);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::begin(radix_tree_node<K, T, Compare>* node) {
    if (node->m_is_leaf) {
        return node;
    }
    assert(!node->m_children.empty());
    return begin(node->m_children.begin()->second);
}

template <typename K, typename T, typename Compare>
T& radix_tree<K, T, Compare>::operator[](const K& lhs) {
    iterator it = find(lhs);

    if (it == end()) {
        std::pair<K, T> val;
        val.first = lhs;

        std::pair<iterator, bool> ret;
        ret = insert(val);

        assert(ret.second == true);

        it = ret.first;
    }

    return it->second;
}

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::greedy_match(const K& key, std::vector<iterator>& vec) {
    vec.clear();

    if (!m_root) {
        return;
    }

    radix_tree_node<K, T, Compare>* node = find_node(key, root(), 0);

    if (node->m_is_leaf) {
        node = node->m_parent;
    }

    greedy_match(node, vec);
}

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::greedy_match(radix_tree_node<K, T, Compare>* node, std::vector<iterator>& vec) {
    if (node->m_is_leaf) {
        vec.push_back(iterator(node));
        return;
    }

    typename std::map<K, radix_tree_node<K, T, Compare>*>::iterator it;

    for (it = node->m_children.begin(); it != node->m_children.end(); ++it) {
        greedy_match(it->second, vec);
    }
}

template <typename K, typename T, typename Compare>
void radix_tree<K, T, Compare>::erase(iterator it) {
    erase(it->first);
}

template <typename K, typename T, typename Compare>
bool radix_tree<K, T, Compare>::erase(const K& key) {
    if (!m_root) {
        return false;
    }

    radix_tree_node<K, T, Compare>* grandparent;
    K nul = radix_substr(key, 0, 0);

    radix_tree_node<K, T, Compare>* child = find_node(key, root(), 0);

    if (!child->m_is_leaf) {
        return false;
    }

    radix_tree_node<K, T, Compare>* parent = child->m_parent;
    parent->m_children.erase(nul);

    delete child;

    m_size--;

    if (parent == root()) {
        return true;
    }

    if (parent->m_children.size() > 1) {
        return true;
    }

    if (parent->m_children.empty()) {
        grandparent = parent->m_parent;
        grandparent->m_children.erase(parent->m_key);
        delete parent;
    } else {
        grandparent = parent;
    }

    if (grandparent == root()) {
        return true;
    }

    if (grandparent->m_children.size() == 1) {
        // merge grandparent with the uncle
        typename std::map<K, radix_tree_node<K, T, Compare>*>::iterator it;
        it = grandparent->m_children.begin();

        radix_tree_node<K, T, Compare>* uncle = it->second;

        if (uncle->m_is_leaf) {
            return true;
        }

        uncle->m_depth = grandparent->m_depth;
        uncle->m_key = radix_join(grandparent->m_key, uncle->m_key);
        uncle->m_parent = grandparent->m_parent;

        grandparent->m_children.erase(it);

        grandparent->m_parent->m_children.erase(grandparent->m_key);
        grandparent->m_parent->m_children[uncle->m_key] = uncle;

        delete grandparent;
    }

    return true;
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::append(radix_tree_node<K, T, Compare>* parent,
                                                                  const value_type& val) {
    K nul = radix_substr(val.first, 0, 0);
    radix_tree_node<K, T, Compare>* node_c;

    int depth = parent->m_depth + radix_length(parent->m_key);
    int len = radix_length(val.first) - depth;

    if (len == 0) {
        node_c = new radix_tree_node<K, T, Compare>(val, m_predicate);

        node_c->m_depth = depth;
        node_c->m_parent = parent;
        node_c->m_key = nul;
        node_c->m_is_leaf = true;

        parent->m_children[nul] = node_c;

        return node_c;
    } else {
        node_c = new radix_tree_node<K, T, Compare>(val, m_predicate);

        K key_sub = radix_substr(val.first, depth, len);

        parent->m_children[key_sub] = node_c;

        node_c->m_depth = depth;
        node_c->m_parent = parent;
        node_c->m_key = key_sub;

        auto* node_cc = new radix_tree_node<K, T, Compare>(val, m_predicate);
        node_c->m_children[nul] = node_cc;

        node_cc->m_depth = depth + len;
        node_cc->m_parent = node_c;
        node_cc->m_key = nul;
        node_cc->m_is_leaf = true;

        return node_cc;
    }
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::prepend(radix_tree_node<K, T, Compare>* node,
                                                                   const value_type& val) {
    const int len1 = radix_length(node->m_key);
    const int len2 = radix_length(val.first) - node->m_depth;

    int count{};
    for (count = 0; count < len1 && count < len2; count++) {
        if (!(node->m_key[count] == val.first[count + node->m_depth])) {
            break;
        }
    }

    assert(count != 0);

    node->m_parent->m_children.erase(node->m_key);

    auto* node_a = new radix_tree_node<K, T, Compare>(m_predicate);

    node_a->m_parent = node->m_parent;
    node_a->m_key = radix_substr(node->m_key, 0, count);
    node_a->m_depth = node->m_depth;
    node_a->m_parent->m_children[node_a->m_key] = node_a;

    node->m_depth += count;
    node->m_parent = node_a;
    node->m_key = radix_substr(node->m_key, count, len1 - count);
    node->m_parent->m_children[node->m_key] = node;

    K nul = radix_substr(val.first, 0, 0);
    if (count == len2) {
        auto* node_b = new radix_tree_node<K, T, Compare>(val, m_predicate);

        node_b->m_parent = node_a;
        node_b->m_key = nul;
        node_b->m_depth = node_a->m_depth + count;
        node_b->m_is_leaf = true;
        node_b->m_parent->m_children[nul] = node_b;

        return node_b;
    } else {
        auto* node_b = new radix_tree_node<K, T, Compare>(m_predicate);

        node_b->m_parent = node_a;
        node_b->m_depth = node->m_depth;
        node_b->m_key = radix_substr(val.first, node_b->m_depth, len2 - count);
        node_b->m_parent->m_children[node_b->m_key] = node_b;

        auto* node_c = new radix_tree_node<K, T, Compare>(val, m_predicate);

        node_c->m_parent = node_b;
        node_c->m_depth = radix_length(val.first);
        node_c->m_key = nul;
        node_c->m_is_leaf = true;
        node_c->m_parent->m_children[nul] = node_c;

        return node_c;
    }
}

template <typename K, typename T, typename Compare>
std::pair<typename radix_tree<K, T, Compare>::iterator, bool> radix_tree<K, T, Compare>::insert(const value_type& val) {
    if (!m_root) {
        K nul = radix_substr(val.first, 0, 0);

        m_root.reset(new radix_tree_node<K, T, Compare>(m_predicate));
        m_root->m_key = nul;
    }

    radix_tree_node<K, T, Compare>* node = find_node(val.first, root(), 0);

    if (node->m_is_leaf) {
        return std::pair<iterator, bool>(iterator{node}, false);
    }
    if (node == root()) {
        m_size++;
        return std::pair<iterator, bool>(iterator{append(root(), val)}, true);
    }
    m_size++;
    int len = radix_length(node->m_key);
    K key_sub = radix_substr(val.first, node->m_depth, len);

    if (key_sub == node->m_key) {
        return std::pair<iterator, bool>(iterator{append(node, val)}, true);
    }
    return std::pair<iterator, bool>(iterator{prepend(node, val)}, true);
}

template <typename K, typename T, typename Compare>
typename radix_tree<K, T, Compare>::iterator radix_tree<K, T, Compare>::find(const K& key) {
    if (!m_root) {
        return iterator(nullptr);
    }

    radix_tree_node<K, T, Compare>* node = find_node(key, root(), 0);

    // if the node is a internal node, return nullptr
    if (!node->m_is_leaf) {
        return iterator(nullptr);
    }

    return iterator(node);
}

template <typename K, typename T, typename Compare>
radix_tree_node<K, T, Compare>* radix_tree<K, T, Compare>::find_node(const K& key, radix_tree_node<K, T, Compare>* node,
                                                                     int depth) {
    if (node->m_children.empty()) {
        return node;
    }

    typename radix_tree_node<K, T, Compare>::it_child it;
    const int len_key = radix_length(key) - depth;

    for (it = node->m_children.begin(); it != node->m_children.end(); ++it) {
        if (len_key == 0) {
            if (it->second->m_is_leaf) {
                return it->second;
            }
            continue;
        }

        if (!it->second->m_is_leaf && key[depth] == it->first[0]) {
            int len_node = radix_length(it->first);
            K key_sub = radix_substr(key, depth, len_node);

            if (key_sub == it->first) {
                return find_node(key, it->second, depth + len_node);
            }
            return it->second;
        }
    }

    return node;
}

/*

(root)
|
|---------------
|       |      |
abcde   bcdef  c
|   |   |      |------
|   |   $3     |  |  |
f   ge         d  e  $6
|   |          |  |
$1  $2         $4 $5

find_node():
  bcdef  -> $3
  bcdefa -> bcdef
  c      -> $6
  cf     -> c
  abch   -> abcde
  abc    -> abcde
  abcde  -> abcde
  abcdef -> $1
  abcdeh -> abcde
  de     -> (root)


(root)
|
abcd
|
$

(root)
|
$

*/
