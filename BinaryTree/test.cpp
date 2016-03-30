#include "catch.hpp"
#include <cassert>
#include <sstream>

using namespace std;

class TreeNode
{
private:
    double    value;
    TreeNode* left;
    TreeNode* right;
    int height;
public:
    TreeNode(double val);
    string toString();
    void addRecursive(double newvalue, TreeNode* &currentNode);
    void removeRecursive(double delval, TreeNode* &currentNode);
    bool containsRecursive(double checkvalue);
    void fixHeight();
    int balanceFactor();
    void rotateLeft(TreeNode*&plink);
    void rotateRight(TreeNode*&plink);
    void balance(TreeNode* &plink);

};
TreeNode::TreeNode(double val)
{
    value=val;
    left=nullptr;
    right=nullptr;
    height=0;
}

class BinaryTree  // no duplicates
{
private:
    TreeNode* root;
public:
    BinaryTree();
    void add(double newvalue);
    void remove(double delval);
    void display();  // not helpful for testing
    bool isEmpty();
    bool contains(double checkvalue);

    string toString();

    friend class TreeNode;
};
BinaryTree::BinaryTree()
{
    root=nullptr;
}
void TreeNode::fixHeight()
{
    if(!left && !right)
    {
        height=0;
    }
    else if(left == nullptr)
    {
        height = right->height + 1;
    }
    else if(right == nullptr)
    {
        height = left->height + 1;
    }
    else if(left->height > right->height)
    {
        height = left->height +1;
    }
    else
    {
        height = right->height + 1;
    }
}

int TreeNode::balanceFactor()
{
    return (left? left->height : -1) -
           (right? right->height : -1 );
}
void TreeNode::rotateRight(TreeNode*&plink)
{
    TreeNode* pivot = left;
    TreeNode* pivotRight = pivot->right;
    pivot->right = this;
    left = pivotRight;
    plink = pivot; 
    pivot->fixHeight();
    fixHeight();
}

void TreeNode::rotateLeft(TreeNode *&plink)
{
    TreeNode* pivot = right;
    TreeNode* pivotLeft = pivot->left;
    pivot->left = this;
    right = pivotLeft;
    plink = pivot;
    pivot->fixHeight();
    fixHeight();
}

void TreeNode::balance(TreeNode* &plink)
{
    if(balanceFactor()>1 && left->balanceFactor() == 1)
    {
        rotateRight(plink);
        //right right
    }
    if(balanceFactor()>1 && left->balanceFactor() == -1)
    {
        left->rotateLeft(left);
        rotateRight(plink);
        //right left
    }
    if(balanceFactor()<-1 && right->balanceFactor() == 1)
    {
        right->rotateRight(right);
        rotateLeft(plink);
        //left right
    }
    if(balanceFactor()<-1 && right->balanceFactor() == -1)
    {
        rotateLeft(plink);
        //left left
    }
    else return;
}

void TreeNode::addRecursive(double newvalue, TreeNode* &currentNode)
{
    if(newvalue>value)
    {
        if(right==nullptr)
        {
            right = new TreeNode(newvalue);
        }
        else
        {
            right->addRecursive(newvalue,right);
        }
    }
    if(newvalue<value)
    {
        if(left==nullptr)
        {
            left = new TreeNode(newvalue);
        }
        else
        {
            left->addRecursive(newvalue,left);
        }
    }
    fixHeight();
    balance(currentNode);

}
void BinaryTree::add(double newvalue)
{
    if(root==nullptr)
    {
        root = new TreeNode(newvalue);
        return;
    }
    root->addRecursive(newvalue, root);

}

void TreeNode::removeRecursive(double delval, TreeNode* &currentNode)
{
    if(delval==value && right==nullptr && left==nullptr)
    {
        currentNode=nullptr;
        delete this;
        return;
    }
    if(delval==value && right!=nullptr && left==nullptr)
    {
        currentNode = right;
        right = nullptr;
        delete this;
        return;
    }
    if(delval==value)
    {
        TreeNode* replacementNode= currentNode;
        if(replacementNode->left!=nullptr)
        {
            replacementNode=replacementNode->left;
            while(replacementNode->right!=nullptr)
            {
                replacementNode=replacementNode->right;
            }
        }
        value=replacementNode->value;
        if(replacementNode->left !=nullptr)
        {
            replacementNode = replacementNode->left;
        }
        replacementNode->removeRecursive(replacementNode->value, replacementNode);
    }
    if(delval>value && right!=nullptr)
    {
        right->removeRecursive(delval, right);
    }
    if(delval<value && left!=nullptr)
    {
        left->removeRecursive(delval, left);
    }
    fixHeight();
    balance(currentNode);
}
void BinaryTree::remove(double delval)
{
    if(root!=nullptr)
    {
        root->removeRecursive(delval, root);
    }
}

void BinaryTree::display()  // not helpful for testing
{

}

bool BinaryTree::isEmpty()
{
    if(root==nullptr)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool TreeNode::containsRecursive(double checkvalue)
{
    if(checkvalue==value)
    {
        return true;
    }
    if(checkvalue>value)
    {
        if(right==nullptr)
        {
            return false;
        }
        return right->containsRecursive(checkvalue);
    }
    if(checkvalue<value)
    {
        if(left==nullptr)
        {
            return false;
        }
        return left->containsRecursive(checkvalue);
    }
    return false;
}

bool BinaryTree::contains(double checkvalue)
{
    if(root==nullptr)
    {
        return false;
    }
    return root->containsRecursive(checkvalue);
}
string BinaryTree::toString()
{
    if (root != nullptr)
    {
        return root->toString();
    }

    return "";
}

string TreeNode::toString() // recursive part
{
    string leftString = (left == nullptr) ? "" : (left->toString() + ",");

    string rightString;

    if (right != nullptr)
    {
        rightString = "," + right->toString();

        stringstream ss;

        ss << 324 << "," << 400;

        string foo = ss.str();
    }
    stringstream ss;
    ss<<"("<<leftString<<value<<rightString<<")";
    string foo = ss.str();
    return foo;
}

#include <iostream>

using namespace std;

/*
int main()
{
    BinaryTree tree;
    tree.add(13);

   // REQUIRE(tree.toString() == "(13)");

    cout << tree.toString() << endl;

    tree.add(4);

   // REQUIRE(tree.toString() == "((4),13)");
    cout << tree.toString() << endl;
    tree.add(27);

   // REQUIRE(tree.toString() == "4, 13, 27");
    cout << tree.toString() << endl;
    tree.add(14);

   // REQUIRE(tree.toString() == "4,13,14,27");
    cout << tree.toString() << endl;
    return 0;
}
*/

TEST_CASE( "various tests for binary trees", "[binaryTree]" ) {

    BinaryTree tree;

    SECTION( "isEmpty tests" ) {

        REQUIRE( tree.isEmpty() == true );

        tree.add(37);

        REQUIRE( tree.isEmpty() == false );

    }

    SECTION( "contains works on empty tree" ) {

        REQUIRE(tree.contains(42) == false);

    }

    SECTION( "adding and removing one item works" ) {

        tree.add(42);

        REQUIRE(tree.contains(42));

        REQUIRE(tree.contains(23) == false);

        tree.remove(42);

        REQUIRE(tree.contains(42) == false);

        REQUIRE(tree.isEmpty() == true);

    }

    SECTION( "toString on empty tree is empty")
    {
        REQUIRE(tree.toString() == "");
    }

    SECTION( "toString works on non-empty tree")
    {
        tree.add(13);

        REQUIRE(tree.toString() == "(13)");

        tree.add(4);

        REQUIRE(tree.toString() == "((4),13)");

        tree.add(27);

        REQUIRE(tree.toString() == "((4),13,(27))");

        tree.add(14);

        REQUIRE(tree.toString() == "((4),13,((14),27))");
    }

    SECTION( "add/remove multiple values")
    {
        REQUIRE(tree.isEmpty() == true);
        tree.add(1);
        tree.add(2);
        REQUIRE(tree.isEmpty() == false);
        REQUIRE(tree.contains(1) == true);
        REQUIRE(tree.contains(2) == true);
        tree.remove(1);
        REQUIRE(tree.contains(1)==false);
        tree.remove(2);
        REQUIRE(tree.isEmpty()== true);
    }

    SECTION( "contains/toString match")
    {
        REQUIRE(tree.toString() == "");
        REQUIRE(tree.isEmpty()== true);
        tree.add(7);
        tree.add(5);
        tree.add(6);
        REQUIRE(tree.contains(7)== true);
        REQUIRE(tree.contains(5)== true);
        REQUIRE(tree.contains(6)== true);
        REQUIRE(tree.toString()=="((5),6,(7))");
    }

    SECTION( "balance function test")
    {
        tree.add(1);
        tree.add(2);
        tree.add(3);
        tree.add(4);
        tree.add(7);
        tree.add(9);
        tree.add(5);
        tree.add(8);
        REQUIRE(tree.toString()=="(((1),2,(3)),4,((5),7,((8),9)))");
    }

}

