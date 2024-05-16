
#ifndef TREESTANDARDIZER_H_
#define TREESTANDARDIZER_H_

#include "lexAnalyser.h"
#include "treeNode.h"

using namespace std;

class TreeStandardizer {
    void standardize(treeNode*);
    void LET(treeNode*);
    void WHERE(treeNode*);
    void WITHIN(treeNode*);
    void REC(treeNode*);
    void FCNFORM(treeNode*);
    void LAMBDA(treeNode*);
    void SIMULDEF(treeNode*);
    void AT(treeNode*);

public:
    TreeStandardizer(treeNode* root);
    virtual ~TreeStandardizer();
};

#endif 
