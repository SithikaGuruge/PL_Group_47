#include "TreeStandardizer.h"

TreeStandardizer::TreeStandardizer(treeNode* root) {
    // Constructs a TreeStandardizer object with the given root node and standardizes the tree.
    standardize(root);
}

TreeStandardizer::~TreeStandardizer() {
    // Destructor for TreeStandardizer object. No explicit clean-up needed.
}

void TreeStandardizer::LET(treeNode* root){
    // Standardizes the LET node in the tree structure.
    // Transforms the structure from:
    //          LET              GAMMA
    //        /     \           /     \
    //     EQUAL     P   ->   LAMBDA   E
    //    /   \             /    \
    //   X     E           X      P

    treeNode* pNode;

    root->type = treeNode::GAMMA;
    root->nodeString = "gamma";
    root->child->type = treeNode::LAMBDA;
    root->child->nodeString = "lambda";
    pNode = root->child->sibling;
    root->child->sibling = root->child->child->sibling;
    root->child->child->sibling = pNode;
}

void TreeStandardizer::WHERE(treeNode* root){
    // Standardizes the WHERE node in the tree structure.
    // Transforms the structure from:
    //          WHERE              GAMMA
    //          /   \             /     \
    //         P    EQUAL   ->  LAMBDA    E
    //              /   \       /   \
    //             X     E     X     P

    treeNode* pNode, *xNode, *eNode;

    root->type = treeNode::GAMMA;
    root->nodeString = "gamma";
    pNode = root->child;
    xNode = root->child->sibling->child;
    eNode = root->child->sibling->child->sibling;
    pNode->sibling = NULL;
    xNode->sibling = NULL;
    root->child = new treeNode();
    root->child->type = treeNode::LAMBDA;
    root->child->nodeString = "lambda";
    root->child->sibling = eNode;
    root->child->child = xNode;
    xNode->sibling = pNode;
}

void TreeStandardizer::WITHIN(treeNode* root){
    // Standardizes the WITHIN node in the tree structure.
    // Transforms the structure from:
    //           WITHIN                  EQUAL
    //          /      \                /     \
    //        EQUAL   EQUAL    ->      X2     GAMMA
    //       /    \   /    \                  /    \
    //      X1    E1 X2    E2               LAMBDA  E1
    //                                      /    \
    //                                     X1    E2

    treeNode *x1Node, *x2Node, *e1Node;

    root->type = treeNode::BINDING;
    x1Node = root->child->child;
    x2Node = root->child->sibling->child;
    e1Node = x1Node->sibling;
    x1Node->sibling = x2Node->sibling;
    x2Node->sibling = new treeNode();
    x2Node->sibling->type = treeNode::GAMMA;
    x2Node->sibling->nodeString = "gamma";
    x2Node->sibling->child = new treeNode();
    x2Node->sibling->child->type = treeNode::LAMBDA;
    x2Node->sibling->child->nodeString = "lambda";
    x2Node->sibling->child->sibling = e1Node; 
    x2Node->sibling->child->child = x1Node; 
    root->child = x2Node;
}

void TreeStandardizer::REC(treeNode* root){
    // Standardizes the REC node in the tree structure.
    // Transforms the structure from:
    //        REC                 EQUAL
    //         |                 /     \
    //       EQUAL     ->       X     GAMMA
    //      /     \                   /    \
    //     X       E                YSTAR  LAMBDA
    //                                     /     \
    //                                    X       E

    treeNode *xNode, *eNode, *gammaNode;

    root->type = treeNode::BINDING;
    xNode = root->child->child;
    eNode = root->child->child->sibling;
    xNode->sibling = NULL;
    root->child = xNode;
    gammaNode = new treeNode();
    gammaNode->type = treeNode::GAMMA;
    gammaNode->nodeString = "gamma";
    xNode->sibling = gammaNode;
    gammaNode->child = new treeNode();
    gammaNode->child->type = treeNode::YSTAR;
    gammaNode->child->nodeString = "<Y*>";
    gammaNode->child->sibling = new treeNode();
    gammaNode->child->sibling->type = treeNode::LAMBDA;
    gammaNode->child->sibling->nodeString = "lambda";
    gammaNode->child->sibling->child = new treeNode();
    gammaNode->child->sibling->child->type = xNode->type;
    gammaNode->child->sibling->child->nodeString = xNode->nodeString;
    gammaNode->child->sibling->child->sibling = eNode;
}

void TreeStandardizer::FCNFORM(treeNode* root){
    // Standardizes the FCN_FORM node in the tree structure.
    // Transforms the structure from:
    //        FCN_FORM                EQUAL
    //       /   |   \              /    \
    //      P    V+   E    ->      P     +LAMBDA
    //                                    /     \
    //                                    V     .E

    treeNode *vNode, *lambdaNode;

    root->type = treeNode::BINDING;
    root->nodeString = "=";
    vNode = root->child;
    while (vNode->sibling->sibling != NULL){
        lambdaNode = new treeNode();
        lambdaNode->type = treeNode::LAMBDA;
        lambdaNode->nodeString = "lambda";
        lambdaNode->child = vNode->sibling;
        vNode->sibling = lambdaNode;
        vNode = lambdaNode->child;
    }
}

void TreeStandardizer::LAMBDA(treeNode* root){
    // Standardizes the LAMBDA node in the tree structure.
    // Transforms the structure from:
    //     LAMBDA        ++LAMBDA
    //      /   \   ->   /    \
    //     V++   E      V     .E

    treeNode *vNode, *lambdaNode;

    vNode = root->child;
    while (vNode->sibling->sibling != NULL){
        lambdaNode = new treeNode();
        lambdaNode->type = treeNode::LAMBDA;
        lambdaNode->nodeString = "lambda";
        lambdaNode->child = vNode->sibling;
        vNode->sibling = lambdaNode;
        vNode = vNode->sibling;
    }
}

void TreeStandardizer::SIMULDEF(treeNode* root){
    // Standardizes the SIMULTDEF (AND) node in the tree structure.
    // Transforms the structure from:
    //    SIMULTDEF(AND)            EQUAL
    //             |               /     \
    //           EQUAL++  ->     COMMA   TAU
    //           /   \             |      |
    //          X     E           X++    E++

    treeNode* tauNode, *commaNode, **currentTau, **currentComma;
    treeNode* newNode;

    root->type = treeNode::BINDING;
    root->nodeString = "=";
    newNode = root->child;
    tauNode = new treeNode();
    commaNode = new treeNode();
    tauNode->type = treeNode::TAU;
    tauNode->nodeString = "tau";
    tauNode->child = NULL;
    tauNode->sibling = NULL;
    commaNode->type = treeNode::COMMA;
    commaNode->nodeString = ",";
    commaNode->child = NULL;
    commaNode->sibling = NULL;
    root->child = commaNode;
    root->child->sibling = tauNode;
    currentTau = &(tauNode->child);
    currentComma = &(commaNode->child);
    while (newNode != NULL){
        *currentTau = newNode->child->sibling;
        newNode->child->sibling = NULL;
        *currentComma = newNode->child;
        newNode = newNode->sibling;
        currentComma = &((*currentComma)->sibling);
        currentTau = &((*currentTau)->sibling);
    }
}

void TreeStandardizer::AT(treeNode* root){
    // Standardizes the AT node in the tree structure.
    // Transforms the structure from:
    //         AT              GAMMA
    //       / | \    ->       /    \
    //      E1 N E2          GAMMA   E2
    //                       /    \
    //                      N     E1

    treeNode* e1Node, *nNode, *e2Node;

    e1Node = root->child;
    nNode = e1Node->sibling;
    e2Node = nNode->sibling;
    root->type = treeNode::GAMMA;
    root->nodeString = "gamma";
    root->child = new treeNode();
    root->child->type = treeNode::GAMMA;
    root->child->nodeString = "gamma";
    root->child->sibling = e2Node;
    root->child->child = nNode;
    nNode->sibling = NULL;
    root->child->child->sibling = e1Node;
    e1Node->sibling = NULL;
}

void TreeStandardizer::standardize(treeNode* root){
    if (root->child != NULL)
        standardize(root->child);
    if (root->sibling != NULL)
        standardize (root->sibling);

    switch(root->type){
    case treeNode::LET:
        LET(root);
        break;
    case treeNode::WHERE:
        WHERE(root);
        break;
    case treeNode::WITHIN:{
        WITHIN(root);
        break;
    }
    case treeNode::REC:{
        REC(root);
        break;
    }
    case treeNode::FCN_FORM:{
        FCNFORM(root);
        break;
    }
    case treeNode::LAMBDA:
        LAMBDA(root);
        break;
    case treeNode::AND:
        SIMULDEF(root);
        break;
    case treeNode::AT:
        AT(root);
        break;
    default:
        // Node types we do NOT standardize:
        // OR
        // PLUS
        // MINUS
        // MULT
        // DIV
        // EXP
        // GR
        // GE
        // LS
        // LE
        // EQ
        // NE
        // NOT
        // NEG
        break;
    }
}
