#include <iostream>
#include "Profiler.h"

struct Node {
    int data = 0;
    Node* left = nullptr;
    Node* right = nullptr;
    int size = 0;
};

Node *buildTree(int arr[], int left, int right) {
    if (left > right) return nullptr;
    Node *root = new Node();
    root->size = right-left+1;
    int mid = (left+right)/2;
    root->data = arr[mid];
    root->left = buildTree(arr, left, mid-1);
    root->right = buildTree(arr, mid+1, right);
    return root;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
