#pragma once

struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
    int x;
    int y;
    TreeNode(int v) : val(v), left(nullptr), right(nullptr), x(0), y(0) {}
};

class BinaryTree {
public:
    BinaryTree() : root(nullptr) {}
    ~BinaryTree();
    void insert(int v);
    void clear();
    void assignPositions();
    TreeNode* getRoot() const { return root; }
private:
    TreeNode* root;
    void deleteRec(TreeNode* node);
    void assignPositionsRec(TreeNode* node, int depth, int& currentX, int hSpacing, int vSpacing);
};
