
#include <cstddef>
#include <cstdio>
#include <memory>
#include <utility>
#include <array>
#include <map>


template<typename T>
using Rc = std::shared_ptr<T>;

template <typename K, typename V>
struct AVLNode {
    int height;
    K key;
    V val;
    Rc<AVLNode<K, V>> left, right;

    AVLNode(int height, K key, V val, Rc<AVLNode<K, V>> left, Rc<AVLNode<K, V>> right):
        height{height},
        key{std::move(key)},
        val{std::move(val)},
        left{std::move(left)},
        right{std::move(right)} {}

    AVLNode(K key, V val, Rc<AVLNode<K, V>> left=nullptr, Rc<AVLNode<K, V>> right=nullptr):
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


template <typename K, typename V>
inline int get_height(const Rc<AVLNode<K, V>> &node) {
    return node ? node->height : 0;
}


template <typename K, typename V>
inline Rc<AVLNode<K, V>> rewrite_ll(Rc<AVLNode<K, V>> root, Rc<AVLNode<K, V>> l, Rc<AVLNode<K, V>> ll) {
    auto &c = l->right;

    root->left = std::move(c);
    root->update_height_with_null_check();

    l->left = std::move(ll);
    l->right = std::move(root);
    l->update_height_without_null_check();

    return l;
}

template <typename K, typename V>
inline Rc<AVLNode<K, V>> rewrite_lr(Rc<AVLNode<K, V>> root, Rc<AVLNode<K, V>> l, Rc<AVLNode<K, V>> lr) {
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


template <typename K, typename V>
inline Rc<AVLNode<K, V>> rewrite_rl(Rc<AVLNode<K, V>> root, Rc<AVLNode<K, V>> r, Rc<AVLNode<K, V>> rl) {
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

template <typename K, typename V>
inline Rc<AVLNode<K, V>> rewrite_rr(Rc<AVLNode<K, V>> root, Rc<AVLNode<K, V>> r, Rc<AVLNode<K, V>> rr) {
    auto &b = r->left;

    root->right = std::move(b);
    root->update_height_with_null_check();

    r->left = std::move(root);
    r->right = std::move(rr);
    r->update_height_without_null_check();

    return r;
}


template <typename K, typename V>
Rc<AVLNode<K, V>> insert(AVLNode<K, V> *root, K key, V val) {
    Rc<AVLNode<K, V>> *path[48];
    int n = 0;
    Rc<AVLNode<K, V>> new_root = nullptr;
    auto ptr = &new_root;

    while (root) {
        if (key < root->key) {
            *ptr = std::make_shared<AVLNode<K, V>>(root->height, root->key, root->val, nullptr, root->right);
            path[n++] = ptr;
            ptr = &(*ptr)->left;
            root = root->left.get();
        }
        else if (root->key < key) {
            *ptr = std::make_shared<AVLNode<K, V>>(root->height, root->key, root->val, root->left, nullptr);
            path[n++] = ptr;
            ptr = &(*ptr)->right;
            root = root->right.get();
        }
        else {
            *ptr = std::make_shared<AVLNode<K, V>>(root->height, std::move(key), std::move(val), root->left, root->right);
            return new_root;
        }
    }

    *ptr = std::make_shared<AVLNode<K, V>>(std::move(key), std::move(val));
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



template <typename K, typename V>
int size(Rc<AVLNode<K, V>> avl) {
    return avl ? 1 + size(avl->left) + size(avl->right) : 0;
}

#include <vector>
#include <random>
#include <unordered_set>


using TestNode = Rc<AVLNode<int, int>>;


void print_avl_helper(TestNode avl) {
    if (avl) {
        print_avl_helper(avl->left);
        printf("%d ", avl->key);
        print_avl_helper(avl->right);
    }
}

void print_avl(TestNode avl) {
    print_avl_helper(avl);
    putc('\n', stdout);
    fflush(stdout);
}

namespace _test {

    void assert(bool x, const char *msg) {
        if (!x) {
            fprintf(stderr, "%s\n", msg); fflush(stderr);
        }
    }
    
    void validate_order(TestNode root, int64_t low=INT64_MIN, int64_t high=INT64_MAX) {
        if (root) {
            assert(low < (int64_t)root->key && (int64_t)root->key < high, "order error\n");
            if (root->left) {
                validate_order(root->left, low, root->key);
            }
            if (root->right) {
                validate_order(root->right, root->key, high);
            }
        }
    }

    int validate_height(TestNode root) {
        if (!root) {
            return 0;
        }
        assert(root->height == 1 + std::max(validate_height(root->left), validate_height(root->right)), "height");
        assert(std::abs(get_height(root->left) - get_height(root->right)) <= 1, "balance");
        return root->height;
    }


    void test_insert(int n) {
        std::vector<TestNode> trees(1, nullptr);
        std::default_random_engine e{};

        std::unordered_set<int> numbers_set;
        while (numbers_set.size() < n) {
            numbers_set.insert(e());
        }
        std::vector<int> numbers(numbers_set.begin(), numbers_set.end());
        for (int i = 0; i < n; ++i) {
            auto last = trees.back();
            int key = numbers[i];
            auto new_avl = insert(last.get(), key, 0);
            trees.push_back(new_avl);

            auto avl_with_same_key = insert(new_avl.get(), numbers[std::uniform_int_distribution<int>(0, i)(e)], 0);
            assert(size(avl_with_same_key) == i + 1, "size -- insert same");
            validate_order(avl_with_same_key);
            validate_height(avl_with_same_key);

            for (int j = 0; j < trees.size(); ++j) {
                auto avl = trees[j];
                assert(size(avl) == j, "size");
                validate_order(avl);
                validate_height(avl);
            }
        }
    }
}


int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    _test::test_insert(n);
}

