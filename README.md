# CliTest

This repository is a playground for CLI experiments.

## Binary Trees (brief overview)

A binary tree is a hierarchical data structure where each node has at most two children, commonly named "left" and "right". It's used for searching (BST), heaps, expression parsing, and more.

Properties:
- Each node has 0, 1, or 2 children
- Height: longest path from root to leaf
- Balanced trees (e.g., AVL, Red-Black) maintain O(log n) operations

Simple ASCII diagram:

       1
      / \
     2   3
    / \   \
   4   5   6

Example in-order traversal: 4, 2, 5, 1, 3, 6

## Code examples

### C++ (simple BST insert + inorder traversal)

```cpp
#include <iostream>
struct Node {
    int val;
    Node* left;
    Node* right;
    Node(int v): val(v), left(nullptr), right(nullptr) {}
};

Node* insert(Node* root, int v){
    if(!root) return new Node(v);
    if(v < root->val) root->left = insert(root->left, v);
    else root->right = insert(root->right, v);
    return root;
}

void inorder(Node* root){
    if(!root) return;
    inorder(root->left);
    std::cout << root->val << " ";
    inorder(root->right);
}

int main(){
    Node* root = nullptr;
    int vals[] = {1,2,3,4,5,6};
    for(int v: vals) root = insert(root, v);
    inorder(root); // prints 1 2 3 4 5 6 (depending on insert order)
}
```

### Python (same idea)

```python
class Node:
    def __init__(self, val):
        self.val = val
        self.left = None
        self.right = None

def insert(root, v):
    if root is None:
        return Node(v)
    if v < root.val:
        root.left = insert(root.left, v)
    else:
        root.right = insert(root.right, v)
    return root

def inorder(root):
    if not root: return []
    return inorder(root.left) + [root.val] + inorder(root.right)

if __name__ == '__main__':
    root = None
    for v in [1,2,3,4,5,6]:
        root = insert(root, v)
    print(inorder(root))
```

If you want graphical diagrams, add PNG/SVG files to the repo and reference them here.

*Updated on branch copilot-readme by Copilot CLI on $ts*
