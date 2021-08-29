#include <cstddef>
#include <cstdio>
#include <memory>


enum Color {
    RED = 0,
    BLACK = 1,
};


struct RbNode {
    Color color;
    int key;
    std::shared_ptr<RbNode> left, right;

    RbNode(Color color, int key, std::shared_ptr<RbNode> left, std::shared_ptr<RbNode> right)
        :color{color}, key{key}, left{std::move(left)}, right{std::move(right)} {}
};

using Rb = std::shared_ptr<RbNode>;

Rb mk_rb(Color color, int key, Rb left, Rb right) {
    return std::make_shared<RbNode>(color, key, left, right);
}

Rb balance(Rb &&root) {
    if (root->color == BLACK) {
        if (root->left && root->left->color == RED) {
            if (root->left->left && root->left->left->color == RED) {
                Rb l = (root->left);
                Rb c = (l->right);
                root->left = c;
                l->right = root;
                l->left->color = BLACK;
                return l;
            }
            if (root->left->right && root->left->right->color == RED) {
                Rb l = (root->left);
                Rb lr = (l->right);
                Rb b = (lr->left);
                Rb c = (lr->right);
                l->right = b;
                root->left = c;
                lr->left = l;
                lr->right = root;
                lr->left->color = BLACK;
                return lr;
            }
        }
        if (root->right && root->right->color == RED) {
            if (root->right->left && root->right->left->color == RED) {
                Rb r = (root->right);
                Rb rl = (r->left);
                Rb b = (rl->left);
                Rb c = (rl->right);
                root->right = b;
                r->left = c;
                rl->left = root;
                rl->right = r;
                rl->right->color = BLACK;
                return rl;
            }
            if (root->right->right && root->right->right->color == RED) {
                Rb r = (root->right);
                Rb b = (r->left);
                root->right = b;
                r->left = root;
                r->right->color = BLACK;
                return r;
            }
        }
    }
    return root;
}


Rb insert_helper(Rb root, int key) {
    if (!root) {
        return mk_rb(RED, key, nullptr, nullptr);
    }
    else {
        if (key < root->key) {
            Rb new_root = mk_rb(root->color, root->key, insert_helper(root->left, key), root->right);
            return balance(std::move(new_root));
        }
        else if (root->key < key) {
            Rb new_root = mk_rb(root->color, root->key, root->left, insert_helper(root->right, key));
            return balance(std::move(new_root));
        }
        else {
            Rb new_root = mk_rb(RED, key, root->left, root->right);
            return new_root;
        }
    }
}


Rb insert(Rb root, int key) {
    Rb new_root = insert_helper(std::move(root), key);
    new_root->color = BLACK;
    return new_root;
}


Rb remove_helper(Rb root, int key) {
    if (!root) {
        return nullptr;
    }
    else {
        if (key < root->key) {
            Rb new_root = mk_rb(root->color, root->key, remove_helper(root->left, key), root->right);
            return new_root;
        }
        else if (root->key < key) {
            Rb new_root = mk_rb(root->color, root->key, root->left, remove_helper(root->right, key));
            return new_root;
        }
        else {
            if (root->left && root->right) {

            }
        }
    }
}

std::pair<Rb, RbNode*> remove_min(Rb root) {
    if (!root->left) {
        return {root->right, root.get()};
    }
    else {
        auto [rb, removed] = remove_min(root->left);
        Rb new_root = mk_rb(root->color, root->key, rb, root->right);
        return {new_root, removed};
    }
}


int size(Rb rb) {
    return rb ? 1 + size(rb->left) + size(rb->right) : 0;
}

#include <vector>
#include <random>

void print_rb_helper(Rb rb) {
    if (rb) {
        print_rb_helper(rb->left);
        printf("%d ", rb->key);
        print_rb_helper(rb->right);
    }
}

void print_rb(Rb rb) {
    print_rb_helper(rb);
    putc('\n', stdout);
    fflush(stdout);
}

namespace _test {

    void assert(bool x, const char *msg) {
        if (!x) {
            fprintf(stderr, "%s\n", msg); fflush(stderr);
        }
    }
    
    void validate_order(Rb root, int64_t low=INT64_MIN, int64_t high=INT64_MAX) {
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


    void rb_attr2(Rb root) {
        if (root) {
            assert(root->color == BLACK, "attr2");
        }
    }

    void rb_attr4(Rb root) {
        if (root) {
            if (root->color == RED) {
                if (root->left) {
                    assert(root->left->color == BLACK, "attr4");
                }
                if (root->right) {
                    assert(root->right->color == BLACK, "attr4");
                }
            }
            rb_attr4(root->left);
            rb_attr4(root->right);
        }
    }

    void rb_attr5_helper(Rb root, int n_black, int &n_black_expect) {
        if (!root) {
            n_black += 1;
            if (n_black_expect < 0) {
                n_black_expect = n_black;
            }
            else {
                assert(n_black == n_black_expect, "attr5");
            }
        }
        else {
            n_black += root->color == BLACK;
            rb_attr5_helper(root->left, n_black, n_black_expect);
            rb_attr5_helper(root->right, n_black, n_black_expect);
        }
    }

    void rb_attr5(Rb root) {
        int n_black_expect = 01;
        rb_attr5_helper(root, 0, n_black_expect);
    }


    void test_insert(int n) {
        std::vector<Rb> trees(1, Rb());
        std::default_random_engine e{};
        for (int i = 0; i < n; ++i) {
            Rb last = trees.back();
            int key = e();
            Rb new_rb = insert(last, key);
            trees.push_back(new_rb);
            for (int j = 0; j < trees.size(); ++j) {
                Rb rb = trees[j];
                assert(size(rb) == j, "size");
                validate_order(rb);
                rb_attr2(rb);
                rb_attr4(rb);
                rb_attr4(rb);
            }
        }
    }
}





int main(int argc, char **argv) {
    int n = atoi(argv[1]);
    _test::test_insert(n);
    Rb a = mk_rb(BLACK, 0, nullptr, nullptr);
    Rb b = insert(a, 1);
    Rb c = insert(b, 2);
    printf("%d %d %d\n", size(a), size(b), size(c));
}