/*
 *  Author: Morar Dragoș Bogdan
 *  Group: 30424
 *  Lab: 8
 *  Subject: Dynamic order statistics
 *      Operations:
 *  BUILD_TREE(n): divide and conquer on 1..n, picks middle as root -> O(n) time.
 *  Each node stores its subtree size (right - left + 1).
 *  OS_SELECT(tree, i): compares i with left->size + 1 and goes into one subtree
 *  -> O(log n) per call (similar to binary search / quickselect).
 *  OS_DELETE(tree, i): normal BST delete by rank, uses inorder successor when a
 *  node has two children. Size is decremented on the way back up the recursion
 *  so it stays O(log n). No rebalancing needed since we only delete and BST
 *  deletion never increases tree height.
 *      Chart (n = 100..10000, step 100, 5 runs):
 *  BUILD grows linearly -> O(n), much lower than the other two.
 *  SELECT (n calls) and DELETE (n calls) both grow as O(n log n).
 *  DELETE is slightly above SELECT because it also updates links, decrements
 *  sizes and handles the successor case.
 *      Bonus (AVL):
 *  Same order statistics tree but with height and left/right rotations on insert
 *  and delete so it stays balanced even when inserting 1..11 in sorted order.
 *  Rotations update size and height in O(1).
 */

#include <iostream>
#include "Profiler.h"

Profiler profiler("DynamicOrderStatistics");
int currentN = 0;

const char OP_BUILD[] = "BUILD";
const char OP_SELECT[] = "SELECT";
const char OP_DELETE[] = "DELETE";

struct Node {
    int data = 0;
    int size = 0;
    int height = 1;
    Node* left = nullptr;
    Node* right = nullptr;
};

int getSize(Node* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->size;
}

Node* buildTree(int left, int right) {
    profiler.countOperation(OP_BUILD, currentN);
    if (left > right) {
        return nullptr;
    }

    int mid = (left + right) / 2;
    Node* node = new Node();
    node->data = mid;
    node->size = right - left + 1;
    node->left = buildTree(left, mid - 1);
    node->right = buildTree(mid + 1, right);
    profiler.countOperation(OP_BUILD, currentN, 6);

    return node;
}

Node* osSelect(Node* root, int i) {
    profiler.countOperation(OP_SELECT, currentN);
    if (root == nullptr) {
        return nullptr;
    }

    int rank = getSize(root->left) + 1;
    profiler.countOperation(OP_SELECT, currentN, 3);
    if (i == rank) {
        return root;
    }

    profiler.countOperation(OP_SELECT, currentN);
    if (i < rank) {
        return osSelect(root->left, i);
    }
    return osSelect(root->right, i - rank);
}

Node* deleteMin(Node* node, int &minData) {
    profiler.countOperation(OP_DELETE, currentN);
    if (node->left == nullptr) {
        minData = node->data;
        Node* rightChild = node->right;
        profiler.countOperation(OP_DELETE, currentN, 2);
        delete node;
        return rightChild;
    }

    node->left = deleteMin(node->left, minData);
    node->size--;
    profiler.countOperation(OP_DELETE, currentN, 2);
    return node;
}

Node* osDelete(Node* root, int i) {
    profiler.countOperation(OP_DELETE, currentN);
    if (root == nullptr) {
        return nullptr;
    }

    int rank = getSize(root->left) + 1;
    profiler.countOperation(OP_DELETE, currentN, 3);
    if (i == rank) {
        profiler.countOperation(OP_DELETE, currentN);
        if (root->left == nullptr) {
            Node* rightChild = root->right;
            profiler.countOperation(OP_DELETE, currentN);
            delete root;
            return rightChild;
        }
        profiler.countOperation(OP_DELETE, currentN);
        if (root->right == nullptr) {
            Node* leftChild = root->left;
            profiler.countOperation(OP_DELETE, currentN);
            delete root;
            return leftChild;
        }

        int succData = 0;
        root->right = deleteMin(root->right, succData);
        root->data = succData;
        root->size--;
        profiler.countOperation(OP_DELETE, currentN, 3);
        return root;
    }

    profiler.countOperation(OP_DELETE, currentN);
    if (i < rank) {
        root->left = osDelete(root->left, i);
    } else {
        root->right = osDelete(root->right, i - rank);
    }
    root->size--;
    profiler.countOperation(OP_DELETE, currentN, 2);
    return root;
}

int getHeight(Node* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

int maxInt(int a, int b) {
    return a > b ? a : b;
}

void updateNode(Node* node) {
    node->size = 1 + getSize(node->left) + getSize(node->right);
    node->height = 1 + maxInt(getHeight(node->left), getHeight(node->right));
}

int getBalance(Node* node) {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

Node* rotateRight(Node* y) {
    Node* x = y->left;
    y->left = x->right;
    x->right = y;
    updateNode(y);
    updateNode(x);
    return x;
}

Node* rotateLeft(Node* x) {
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    updateNode(x);
    updateNode(y);
    return y;
}

Node* rebalance(Node* node) {
    int balance = getBalance(node);

    if (balance > 1) {
        if (getBalance(node->left) < 0) {
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);
    }

    if (balance < -1) {
        if (getBalance(node->right) > 0) {
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);
    }

    return node;
}

Node* avlInsert(Node* root, int key) {
    if (root == nullptr) {
        Node* node = new Node();
        node->data = key;
        node->size = 1;
        node->height = 1;
        return node;
    }

    if (key < root->data) {
        root->left = avlInsert(root->left, key);
    } else {
        root->right = avlInsert(root->right, key);
    }

    updateNode(root);
    return rebalance(root);
}

Node* avlDeleteMin(Node* node, int &minData) {
    if (node->left == nullptr) {
        minData = node->data;
        Node* rightChild = node->right;
        delete node;
        return rightChild;
    }

    node->left = avlDeleteMin(node->left, minData);
    updateNode(node);
    return rebalance(node);
}

Node* avlDelete(Node* root, int i) {
    if (root == nullptr) {
        return nullptr;
    }

    int rank = getSize(root->left) + 1;
    if (i == rank) {
        if (root->left == nullptr) {
            Node* rightChild = root->right;
            delete root;
            return rightChild;
        }
        if (root->right == nullptr) {
            Node* leftChild = root->left;
            delete root;
            return leftChild;
        }

        int succData = 0;
        root->right = avlDeleteMin(root->right, succData);
        root->data = succData;
    } else if (i < rank) {
        root->left = avlDelete(root->left, i);
    } else {
        root->right = avlDelete(root->right, i - rank);
    }

    updateNode(root);
    return rebalance(root);
}

void prettyPrint(Node* root, int level, bool showHeight) {
    if (root == nullptr) {
        return;
    }
    prettyPrint(root->right, level + 1, showHeight);
    for (int i = 0; i < level; i++) {
        std::cout << "    ";
    }
    if (showHeight) {
        std::cout << root->data << "(s=" << root->size << ",h=" << root->height << ")\n";
    } else {
        std::cout << root->data << "(" << root->size << ")\n";
    }
    prettyPrint(root->left, level + 1, showHeight);
}

void freeTree(Node* root) {
    if (root == nullptr) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    delete root;
}

void demo() {
    int n = 11;

    Node* root = buildTree(1, n);
    std::cout << "Initial balanced BST (n = " << n << "), format key(size):\n";
    prettyPrint(root, 0, false);

    std::cout << "\nOS-SELECT demo (3 random indices):\n";
    for (int k = 0; k < 3; k++) {
        int idx = rand() % n + 1;
        Node* found = osSelect(root, idx);
        std::cout << "Index " << idx << " -> key " << found->data << "\n";
    }

    std::cout << "\nOS-SELECT + OS-DELETE demo (3 random deletions):\n";
    int remaining = n;
    for (int k = 0; k < 3; k++) {
        int idx = rand() % remaining + 1;
        Node* found = osSelect(root, idx);
        std::cout << "\nDeleting index " << idx << " (key " << found->data << "):\n";
        root = osDelete(root, idx);
        remaining--;
        prettyPrint(root, 0, false);
    }

    freeTree(root);
}

void demoAvl() {
    int n = 11;

    Node* root = nullptr;
    for (int key = 1; key <= n; key++) {
        root = avlInsert(root, key);
    }

    std::cout << "\n--- Bonus: AVL Tree Demo (n = " << n << ") ---\n";
    std::cout << "AVL tree built via inserting 1.." << n << ", format key(s=size,h=height):\n";
    prettyPrint(root, 0, true);

    std::cout << "\nAVL OS-SELECT demo (3 random indices):\n";
    for (int k = 0; k < 3; k++) {
        int idx = rand() % n + 1;
        Node* found = osSelect(root, idx);
        std::cout << "Index " << idx << " -> key " << found->data << "\n";
    }

    std::cout << "\nAVL OS-SELECT + OS-DELETE demo (3 random deletions):\n";
    int remaining = n;
    for (int k = 0; k < 3; k++) {
        int idx = rand() % remaining + 1;
        Node* found = osSelect(root, idx);
        std::cout << "\nDeleting index " << idx << " (key " << found->data << "):\n";
        root = avlDelete(root, idx);
        remaining--;
        prettyPrint(root, 0, true);
    }

    freeTree(root);
}

void perf() {
    int nrTests = 5;

    for (int n = 100; n <= 10000; n += 100) {
        std::cout << n << " / 10000" << '\n';
        currentN = n;
        for (int t = 0; t < nrTests; t++) {
            Node* root = buildTree(1, n);
            for (int remaining = n; remaining >= 1; remaining--) {
                int idx = rand() % remaining + 1;
                osSelect(root, idx);
                root = osDelete(root, idx);
            }
        }
    }

    profiler.divideValues(OP_BUILD, nrTests);
    profiler.divideValues(OP_SELECT, nrTests);
    profiler.divideValues(OP_DELETE, nrTests);
    profiler.createGroup("ManagementOperations", OP_BUILD, OP_SELECT, OP_DELETE);
}

int main() {
    srand(time(nullptr));
    perf();
    profiler.reset("demo");
    demo();
    demoAvl();
    return 0;
}
