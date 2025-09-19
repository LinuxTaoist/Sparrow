#ifndef __SHARED_BINARY_TREE_H__
#define __SHARED_BINARY_TREE_H__

#include <semaphore.h>
#include <map>
#include <string>

#define SHARED_BTREE_KEY_MAX_LEN    32
#define SHARED_BTREE_VALUE_MAX_LEN  64

struct Node
{
    char key[SHARED_BTREE_KEY_MAX_LEN];
    char value[SHARED_BTREE_VALUE_MAX_LEN];
    uint32_t left;  // 改为偏移量，0表示空
    uint32_t right; // 改为偏移量，0表示空
};

class SharedBinaryTree
{
public:
    SharedBinaryTree(const std::string& filename, size_t size, bool create = true);
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

    // 辅助方法：通过偏移量获取节点指针
    Node* GetNodeByOffset(uint32_t offset) {
        return offset == 0 ? nullptr : (Node*)((char*)mRoot + sizeof(mCurUsedSize) + offset);
    }

    // 辅助方法：获取节点相对于根的偏移量
    uint32_t GetNodeOffset(Node* node) {
        return node == nullptr ? 0 : (uint32_t)((char*)node - (char*)mRoot - sizeof(mCurUsedSize));
    }

    int   OpenMapFile(const std::string& filename, size_t size, bool create);
    Node* CreateNode(const std::string& key, const std::string& value);
    void  GetKeyValue(Node* node, std::map<std::string, std::string>& keyValueMap);
};

#endif // __SHARED_BINARY_TREE_H__
