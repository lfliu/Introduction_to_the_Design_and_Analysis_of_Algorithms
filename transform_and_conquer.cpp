/************************************
** Edition:	Demo
** Author:	Kingsley Chen	
** Date:	2013/07/20
** Purpose:	chapter 6: Transform-and-Conquer Algorithms
************************************/

#include <algorithm>
#include <vector>
#include <tuple>
#include <cstdio>

using std::vector;
using std::pair;
using std::tuple;
using std::sort;
using std::make_pair;
using std::make_tuple;

bool IsElementUnique(int ary[], size_t len)
{
    std::sort(ary, ary + len);

    for (size_t i = 0; i < len - 1; ++i)
    {
        if (ary[i] == ary[i+1])
        {
            return false;
        }
    }

    return true;
}

int ComputeMode(int ary[], size_t len)
{
    std::sort(ary, ary + len);

    int mode = ary[0], modeFrequency = 0;

    // i always points to the first occurence of a element that might be the mode
    for (size_t i = 0; i < len;)
    {
        int runVal = ary[i], runLen = 1;

        while (i + runLen < len && ary[i+runLen] == runVal)
        {
            ++runLen;
        }

        if (runLen > modeFrequency)
        {
            mode = runVal;
            modeFrequency = runLen;
        }

        i += runLen;
    }

    return mode;
}

void Intersection(vector<int>& a, vector<int>& b, vector<int>& commonEle)
{
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());

    for (auto itA = a.cbegin(), itB = b.cbegin(); itA != a.cend() && itB != b.cend();)
    {
        if (*itA == *itB)
        {
            commonEle.push_back(*itA);
            ++itA;
            ++itB;
        }
        else if (*itA < *itB)
        {
            ++itA;
        } 
        else
        {
            ++itB;
        }
    }
}

tuple<bool,pair<int,int>> FindSumMatch(int ary[], size_t len, int val)
{
    sort(ary, ary + len);

    for (size_t l = 0, r = len - 1; l < r;)
    {
        int attemptSum = ary[l] + ary[r];
        if (attemptSum == val)
        {
            return make_tuple(true, make_pair(ary[l], ary[r]));
        }
        else if (attemptSum < val)
        {
            ++l;
        } 
        else /* attemptSum > val */
        {
            --r;
        }
    }

    return make_tuple(false, make_pair(0, 0));            
}

/*
def MaximumOverlappedIntervals(intervals):
    leftEnd = []
    rightEnd = []
    
    for (l, r) in intervals:
        leftEnd.append(l)
        rightEnd.append(r)

    leftEnd.sort()
    rightEnd.sort()

    # len(leftEnd) must equal len(rightEnd)
    l = r = 0
    m = overloap = 0
    while l < len(leftEnd) and r < len(rightEnd):
        if leftEnd[l] < rightEnd[r]:
            overloap += 1
            l += 1
            if overloap > m:
                m = overloap
        else:
            overloap -= 1
            r += 1

    return m
*/

/*
def SolveLinearEquationSystem(coeffMatrix, valVector):
    # create the augmented matrix
    augMatrix = []
    for i in range(0, len(coeffMatrix)):
        augMatrix.append(coeffMatrix[i] + [valVector[i]])
    
    for i in range(0, len(augMatrix) - 1):
        # find row containing largest ith col ele then swap with row i
        # ensure scaling factor will never exceed 1
        # otherwise, round-off error may be bring about due to different magnitude order
        # in subsequent subtraction operations
        pivot = i
        for t in range(i + 1, len(augMatrix)):
            if abs(augMatrix[t][i]) > abs(augMatrix[pivot][i]):
                pivot = t

        if pivot != i:
            augMatrix[i], augMatrix[pivot] = augMatrix[pivot], augMatrix[i]

        for j in range(i + 1, len(augMatrix)):
            # assume a[i,i] is non-zero
            tmp = augMatrix[j][i] / augMatrix[i][i] 
            for k in range(i, len(augMatrix[0])):
                augMatrix[j][k] -= augMatrix[i][k] * tmp

    #backward substitution
    solVector = [0] * len(augMatrix)
    for i in [j for j in range(0, len(augMatrix))][::-1]:
        tmp = 0.0
        for k in [j for j in range(i + 1, len(augMatrix))][::-1]:
            tmp += augMatrix[i][k] * solVector[k]

        solVector[i] = (augMatrix[i][-1] - tmp) / augMatrix[i][i]

    return solVector
*/

struct Node
{
    Node(int val, int hei) : value(val), height(hei),
                             parent(nullptr), children()
    {
    }

    int value;
    Node* children[2];
    Node* parent;
    int height;
};

#if _DEBUG
void VisitTree(const Node* root)
{
    if (root == nullptr)
    {
        return;
    }

    printf_s("value: %d height: %d\n", root->value, root->height);
    VisitTree(root->children[0]);
    VisitTree(root->children[1]);
}
#endif

inline int NodeHeight(const Node* node)
{
    if (nullptr == node)
    {
        return -1;
    } 
    else
    {
        return node->height;
    }
}

/*
	Description:
		recalculates height (a bottom-up way) of all nodes on the path 
        where new node was through and returns first unbalanced node
	Parameters:
		nodeInserted[in,out] - newly inserted node
	Return Value:
		null if no balance property violated
        pointer of the first unbalanced node
*/
Node* EvaluateBalance(Node* nodeInserted)
{
    Node* curr = nodeInserted;
    Node* unbalancedNode = nullptr;

    if (curr->parent && !(curr->parent->children[0] && curr->parent->children[1]))
    {
        while (curr = curr->parent)
        {
            curr->height++;
        
            // check balance factor only if no unbalanced node found
            if (nullptr == unbalancedNode)
            {
                int balanceDiff = NodeHeight(curr->children[0]) - 
                                  NodeHeight(curr->children[1]);
                if (2 == balanceDiff || -2 == balanceDiff)
                {
                    unbalancedNode = curr;
                }
            }
        }
    }
    
    return unbalancedNode;
}

// insert a node into the tree
pair<Node*, bool> Insert(int ele, Node*& root)
{
    // when loop terminates, currPos points to the position where
    // new node should be placed
    Node* parent = nullptr;
    Node** currPos = &root;
    while (*currPos != nullptr)
    {
        parent = *currPos;
        if (ele < (*currPos)->value)
        {
            currPos = &(*currPos)->children[0];
        } 
        else if (ele > (*currPos)->value)
        {
            currPos = &(*currPos)->children[1];
        }
        else
        {
            // element already exists
            return make_pair(*currPos, false);
        }
    }

    Node* node = new Node(ele, 0);
    *currPos = node;
    node->parent = parent;

    // recalculate height and check balance factor of each node on the path
    Node* unb = EvaluateBalance(node);
    
    //TODO: employ rotations to gain rebalance
    if (unb != nullptr)
    {
        // root-pivot left skew
        if (NodeHeight(unb->children[0]) > NodeHeight(unb->children[1]))
        {
            Node* unbPiv = unb->children[0];
            if (NodeHeight(unbPiv->children[0]) > NodeHeight(unbPiv->children[1]))
            {
                // LL skew
                printf_s("node %d and %d LL skew\n", unb->value, unbPiv->value);
            } 
            else
            {
                // LR skew
                printf_s("node %d and %d LR skew\n", unb->value, unbPiv->value);
            }
        }
        else    // root-pivot right skew
        {
            Node* unbPiv = unb->children[1];
            if (NodeHeight(unbPiv->children[0]) < NodeHeight(unbPiv->children[1]))
            {
                // RR
                printf_s("node %d and %d RR skew\n", unb->value, unbPiv->value);
            } 
            else
            {
                // RL
                printf_s("node %d and %d RL skew\n", unb->value, unbPiv->value);
            }
        }
    }

    return make_pair(node, true);
}

Node* ConstructAVLTree(const int ary[], size_t len)
{
    Node* tree = nullptr;
    for (size_t i = 0; i < len; ++i)
    {
        Insert(ary[i], tree);
    }

    return tree;
}

void RotateLeft(Node* node)
{

}

void RotateRight(Node* node)
{

}

void DestroyAVLTree(Node*& root)
{
    if (nullptr == root)
    {
        return;
    }

    Node* leftSubtreeRoot = root->children[0];
    Node* rightSubtreeRoot = root->children[1];

    delete root;
    root = nullptr;

    DestroyAVLTree(leftSubtreeRoot);
    DestroyAVLTree(rightSubtreeRoot);
}