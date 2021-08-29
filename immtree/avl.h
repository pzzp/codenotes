#pragma once

#include <cstddef>
#include <cstdio>
#include <memory>
#include <utility>
#include <optional>


template<typename T>
using Rc = std::shared_ptr<T>;

template <typename K, typename V>
class AVLMap {
    struct AVLNode {
        int height;
        K key;
        V val;
        Rc<AVLNode> left, right;

        AVLNode(int height, K key, V val, Rc<AVLNode> left, Rc<AVLNode> right):
            height{height},
            key{std::move(key)},
            val{std::move(val)},
            left{std::move(left)},
            right{std::move(right)} {}

        AVLNode(K key, V val, Rc<AVLNode> left=nullptr, Rc<AVLNode> right=nullptr):
            height{1 + std::max(left ? left->height : 0, right ? right->height : 0)},
            key{std::move(key)},
            val{std::move(val)},
            left{std::move(left)},
            right{std::move(right)} {}
        
        void update_height_without_null_check() {
            height = std::max(left->height, right->height) + 1;
        }

        void update_height_with_null_check() {
            height = std::max(left ? left->height : 0, right ? right->height : 0) + 1;
        }
    };

private:
    Rc<AVLNode> root;
    size_t _size;

    AVLMap(Rc<AVLNode> root, size_t size): root{std::move(root)}, _size(size) {}

public:
    AVLMap(): root{}, _size{0} {}
    AVLMap<K, V> insert(K key, V val) const {
        auto new_root = insert(root, std::move(key), std::move(val));
        return AVLMap<K, V>{std::move(new_root), _size + 1};
    }

    size_t size() const {
        return _size;
    }

    std::optional<const V&> find(const K &key) const {
        auto cur_root = this->root->get();
        while (cur_root) {
            if (key < cur_root->key) {
                cur_root = cur_root->left->get();
            }
            else if (cur_root->key < key) {
                cur_root = cur_root->right->get();
            }
            else {
                return {cur_root->val};
            }
        }
        return {};
    }

    const V& find_default(const K &key, const V &default_value) const {
        return find(key)->value_or(default_value);
    }

private:
    static inline int get_height(const Rc<AVLNode> &node) {
        return node ? node->height : 0;
    }


    static inline Rc<AVLNode> rewrite_ll(Rc<AVLNode> root, Rc<AVLNode> l, Rc<AVLNode> ll) {
        auto &c = l->right;

        root->left = std::move(c);
        root->update_height_with_null_check();

        l->left = std::move(ll);
        l->right = std::move(root);
        l->update_height_without_null_check();

        return l;
    }

    static inline Rc<AVLNode> rewrite_lr(Rc<AVLNode> root, Rc<AVLNode> l, Rc<AVLNode> lr) {
        auto &b = lr->left;
        auto &c = lr->right;

        root->left = std::move(c);
        root->update_height_with_null_check();

        l->right = std::move(b);
        l->update_height_with_null_check();

        lr->left = std::move(l);
        lr->right = std::move(root);
        lr->update_height_without_null_check();

        return lr;
    }


    static inline Rc<AVLNode> rewrite_rl(Rc<AVLNode> root, Rc<AVLNode> r, Rc<AVLNode> rl) {
        auto &b = rl->left;
        auto &c = rl->right;

        root->right = std::move(b);
        root->update_height_with_null_check();

        r->left = std::move(c);
        r->update_height_with_null_check();

        rl->left = std::move(root);
        rl->right = std::move(r);
        rl->update_height_without_null_check();

        return rl;
    }

    static inline Rc<AVLNode> rewrite_rr(Rc<AVLNode> root, Rc<AVLNode> r, Rc<AVLNode> rr) {
        auto &b = r->left;

        root->right = std::move(b);
        root->update_height_with_null_check();

        r->left = std::move(root);
        r->right = std::move(rr);
        r->update_height_without_null_check();

        return r;
    }


    static Rc<AVLNode> insert(AVLNode *root, K key, V val) {
        Rc<AVLNode> *path[48];
        int n = 0;
        Rc<AVLNode> new_root = nullptr;
        auto ptr = &new_root;

        while (root) {
            if (key < root->key) {
                *ptr = std::make_shared<AVLNode>(root->height, root->key, root->val, nullptr, root->right);
                path[n++] = ptr;
                ptr = &(*ptr)->left;
                root = root->left.get();
            }
            else if (root->key < key) {
                *ptr = std::make_shared<AVLNode>(root->height, root->key, root->val, root->left, nullptr);
                path[n++] = ptr;
                ptr = &(*ptr)->right;
                root = root->right.get();
            }
            else {
                *ptr = std::make_shared<AVLNode>(root->height, std::move(key), std::move(val), root->left, root->right);
                return new_root;
            }
        }

        *ptr = std::make_shared<AVLNode>(std::move(key), std::move(val));
        if (n == 0) {
            return *ptr;
        }

        path[--n]->get()->update_height_with_null_check();
        while (--n >= 0) {
            auto &cur_root = *path[n];
            int old_height = path[n]->get()->height;
            cur_root->update_height_with_null_check();

            if (get_height(cur_root->left) > get_height(cur_root->right) + 1) {
                auto &l = cur_root->left;
                if (get_height(l->left) > get_height(l->right)) {
                    auto cur_root_new = rewrite_ll(std::move(cur_root), std::move(l), std::move(l->left));
                    auto new_height = cur_root_new->height;
                    *path[n] = std::move(cur_root_new);
                    if (new_height == old_height) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
                if (get_height(l->left) < get_height(l->right)) {
                    auto cur_root_new = rewrite_lr(std::move(cur_root), std::move(l), std::move(l->right));
                    auto new_height = cur_root_new->height;
                    *path[n] = std::move(cur_root_new);
                    if (new_height == old_height) {
                        break;
                    }
                    else {
                        continue;
                    }

                }
            }
            if (get_height(cur_root->left) < get_height(cur_root->right) - 1) {
                auto &r = cur_root->right;
                if (get_height(r->left) > get_height(r->right)) {
                    auto cur_root_new = rewrite_rl(std::move(cur_root), std::move(r), std::move(r->left));
                    auto new_height = cur_root_new->height;
                    *path[n] = std::move(cur_root_new);
                    if (new_height == old_height) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
                if (get_height(r->left) < get_height(r->right)) {
                    auto cur_root_new = rewrite_rr(std::move(cur_root), std::move(r), std::move(r->right));
                    auto new_height = cur_root_new->height;
                    *path[n] = std::move(cur_root_new);
                    if (new_height == old_height) {
                        break;
                    }
                    else {
                        continue;
                    }
                }
            }
        }
        return new_root;
    }
};