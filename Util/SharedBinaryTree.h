/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SharedBinaryTree.h
 *  @author     : Xiang.D (dx_65535@163.com)
 *  @version    : 1.0
 *  @brief      : Blog: https://mp.weixin.qq.com/s/eoCPWMGbIcZyxvJ3dMjQXQ
 *  @date       : 2023/12/13
 *
 *
 *  Change History:
 *  <Date>     | <Version> | <Author>       | <Description>
 *---------------------------------------------------------------------------------------------------------------------
 *  2023/12/23 | 1.0.0.1   | Xiang.D        | Create file
 *---------------------------------------------------------------------------------------------------------------------
 *
 */
#ifndef __SHARED_BINARY_TREE_H__
#define __SHARED_BINARY_TREE_H__

#include <semaphore.h>
#include <map>
#include <string>

#define KEY_SIZE    32
#define VALUE_SIZE  64

struct Node
{
    char key[KEY_SIZE];
    char value[VALUE_SIZE];
    Node* left;
    Node* right;
};

class SharedBinaryTree
{
public:
    SharedBinaryTree(const std::string& filename, size_t size);
    ~SharedBinaryTree();

    int GetValue(const std::string& key, std::string& value);
    int SetValue(const std::string& key, const std::string& value);
    void GetAllKeyValues(std::map<std::string, std::string>& keyValueMap);

private:
    int    mHandler;
    void*  mRoot;           // 共享内存根指针
    size_t mSize;           // 共享内存容量
    size_t mCurUsedSize;    // 当前已使用容量
    sem_t  mSemaphore;
    Node*  mFirstNode;      // 第一个数据节点
    std::string mFilename;

    int   OpenMapFile(const std::string& filename, size_t size);
    Node* CreateNode(const std::string& key, const std::string& value);
    void  GetKeyValue(Node* node, std::map<std::string, std::string>& keyValueMap);
};

#endif // __SHARED_BINARY_TREE_H__
