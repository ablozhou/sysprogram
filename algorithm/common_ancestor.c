#include <stdio.h>

typedef struct
{
    int value;
    TreeNode *left;
    TreeNode *right;
} TreeNode;

TreeNode *lowestCommonAncestor(TreeNode *root, TreeNode *p, TreeNode *q)
{
    if (root == NULL)
        return NULL;
    if (root == p || root == q)
    {
        return root;
    }
    TreeNode *left = lowestCommonAncestor(root->left, p, q);
    TreeNode *right = lowestCommonAncestor(root->right, p, q);
    // common ancestor
    if (left && right)
    {
        return root;
    }
    if (left)
    {
        return left;
    }
    if (right)
    {
        return right;
    }
    return NULL;
}
