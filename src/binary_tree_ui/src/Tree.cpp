#include "..\include\Tree.h"
#include <functional>

BinaryTree::~BinaryTree() { clear(); }

void BinaryTree::clear() { deleteRec(root); root = nullptr; }

void BinaryTree::deleteRec(TreeNode* node) {
    if (!node) return;
    deleteRec(node->left);
    deleteRec(node->right);
    delete node;
}

TreeNode* BinaryTree::insertRec(TreeNode* node, int v) {
    if (!node) return new TreeNode(v);
    if (v < node->val) node->left = insertRec(node->left, v);
    else node->right = insertRec(node->right, v);
    return node;
}

void BinaryTree::insert(int v) {
    root = insertRec(root, v);
}

void BinaryTree::assignPositions() {
    int currentX = 0;
    const int hSpacing = 80;
    const int vSpacing = 80;
    assignPositionsRec(root, 0, currentX, hSpacing, vSpacing);
}

void BinaryTree::assignPositionsRec(TreeNode* node, int depth, int& currentX, int hSpacing, int vSpacing) {
    if (!node) return;
    assignPositionsRec(node->left, depth+1, currentX, hSpacing, vSpacing);
    node->x = currentX * hSpacing + 50;
    node->y = depth * vSpacing + 80;
    currentX++;
    assignPositionsRec(node->right, depth+1, currentX, hSpacing, vSpacing);
}
