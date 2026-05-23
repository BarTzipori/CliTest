#include "..\include\Tree.h"
#include <stack>

BinaryTree::~BinaryTree() { clear(); }

void BinaryTree::clear() { deleteRec(root); root = nullptr; }

void BinaryTree::deleteRec(TreeNode* node) {
    if (!node) return;
    std::stack<TreeNode*> st;
    st.push(node);
    while (!st.empty()) {
        TreeNode* n = st.top(); st.pop();
        if (n->left) st.push(n->left);
        if (n->right) st.push(n->right);
        delete n;
    }
}

void BinaryTree::insert(int v) {
    if (!root) { root = new TreeNode(v); return; }
    TreeNode* cur = root;
    while (true) {
        if (v < cur->val) {
            if (cur->left) cur = cur->left;
            else { cur->left = new TreeNode(v); return; }
        } else {
            if (cur->right) cur = cur->right;
            else { cur->right = new TreeNode(v); return; }
        }
    }
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
