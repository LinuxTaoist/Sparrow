/**
 *---------------------------------------------------------------------------------------------------------------------
 *  @copyright Copyright (c) 2022  <dx_65535@163.com>.
 *
 *  @file       : SharedBinaryTree.cpp
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
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "SharedBinaryTree.h"

using namespace std;

#define SPR_LOGD(fmt, args...) printf("%d SharedBinaryTree D: " fmt, __LINE__, ##args)
#define SPR_LOGW(fmt, args...) printf("%d SharedBinaryTree W: " fmt, __LINE__, ##args)
#define SPR_LOGE(fmt, args...) printf("%d SharedBinaryTree E: " fmt, __LINE__, ##args)

SharedBinaryTree::SharedBinaryTree(const string& filename, size_t size, bool create)
    : mRoot(nullptr), mSize(size), mCurUsedSize(0), mFirstNode(nullptr), mFilename(filename)
{
    mHandler = OpenMapFile(filename, size, create);
    if (mHandler == -1) {
        SPR_LOGE("OpenAndCreateFile fail!\n");
    }

    mRoot = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mHandler, 0);
    if (mRoot == MAP_FAILED) {
        close(mHandler);
        mHandler = -1;
        SPR_LOGE("mmap fail! (%s)\n", strerror(errno));
    }

    if (sem_init(&mSemaphore, 1, 1) == -1) {
        SPR_LOGE("sem_init fail! (%s)\n", strerror(errno));
    }

    // mCurUsedSize record at 0 and the length is sizeof(mCurUsedSize)
    mCurUsedSize = *(size_t*)mRoot ? *(size_t*)mRoot : sizeof(mCurUsedSize);

    // init first node
    mFirstNode = (Node*)((char*)mRoot + sizeof(mCurUsedSize));
    if (mFirstNode != nullptr && create) {
        // 初始化根节点的左右子树偏移量为0
        mFirstNode->key[0] = '\0';
        mFirstNode->left = 0;
        mFirstNode->right = 0;
    } else if (mFirstNode == nullptr) {
        SPR_LOGE("mFirstNode is nullptr!\n");
    }

}

SharedBinaryTree::~SharedBinaryTree()
{
    sem_destroy(&mSemaphore);
    if (mRoot != nullptr) {
        munmap(mRoot, mSize);
    }

    if (mHandler != -1) {
        close(mHandler);
        mHandler = -1;
    }
}

int SharedBinaryTree::GetValue(const string& key, string& value)
{
    int ret = -1;
    sem_wait(&mSemaphore);
    Node* pNode = mFirstNode;

    while (pNode != nullptr) {
        if (key == pNode->key) {
            value = pNode->value;
            ret = 0;
            break;
        } else if (key < pNode->key) {
            // 通过偏移量获取左子节点
            pNode = GetNodeByOffset(pNode->left);
        } else {
            // 通过偏移量获取右子节点
            pNode = GetNodeByOffset(pNode->right);
        }
    }

    sem_post(&mSemaphore);
    return ret;
}

int SharedBinaryTree::SetValue(const string& key, const string& value)
{
    if (key.size() >= SHARED_BTREE_KEY_MAX_LEN || value.size() >= SHARED_BTREE_VALUE_MAX_LEN) {
        SPR_LOGW("Lenght out of limit! Limit length: %d %d\n", SHARED_BTREE_KEY_MAX_LEN, SHARED_BTREE_VALUE_MAX_LEN);
        return -1;
    }

    sem_wait(&mSemaphore);
    Node* pNode = mFirstNode;
    if (pNode == nullptr) {
        SPR_LOGE("pNode is nullptr! \n");
        sem_post(&mSemaphore);
        return -1;
    }

    // 如果根节点为空，直接设置根节点
    if (pNode->key[0] == '\0') {
        strncpy(pNode->key, key.c_str(), SHARED_BTREE_KEY_MAX_LEN - 1);
        pNode->key[SHARED_BTREE_KEY_MAX_LEN - 1] = '\0';
        strncpy(pNode->value, value.c_str(), SHARED_BTREE_VALUE_MAX_LEN - 1);
        pNode->value[SHARED_BTREE_VALUE_MAX_LEN - 1] = '\0';
        pNode->left = 0;
        pNode->right = 0;

        mCurUsedSize = sizeof(mCurUsedSize) + sizeof(Node);
        *reinterpret_cast<size_t*>(mRoot) = mCurUsedSize;

        sem_post(&mSemaphore);
        return 0;
    }

    Node* pPrev = nullptr;
    while (pNode != nullptr) {
        if (key == pNode->key) {
            strncpy(pNode->value, value.c_str(), SHARED_BTREE_VALUE_MAX_LEN);
            pNode->value[SHARED_BTREE_VALUE_MAX_LEN - 1] = '\0';
            sem_post(&mSemaphore);
            return 0;
        }

        pPrev = pNode;
        if (key < pNode->key) {
            pNode = GetNodeByOffset(pNode->left);
        } else {
            pNode = GetNodeByOffset(pNode->right);
        }
    }

    mCurUsedSize += sizeof(Node);
    if (mCurUsedSize > mSize) {
        mCurUsedSize -= sizeof(Node);
        SPR_LOGE("Resource ou of limit! mCurUsedSize = %zu, mSize = %zu\n", mCurUsedSize, mSize);
        sem_post(&mSemaphore);
        return -1;
    }

    Node* pNewNode = CreateNode(key, value);
    if (pNewNode == nullptr) {
        SPR_LOGE("pNewNode is nullptr! \n");
        mCurUsedSize -= sizeof(Node);
        sem_post(&mSemaphore);
        return -1;
    }

    // 更新共享内存使用大小
    *reinterpret_cast<size_t*>(mRoot) = mCurUsedSize;

    // 设置父节点的子节点偏移量
    if (key < pPrev->key) {
        pPrev->left = GetNodeOffset(pNewNode);
    } else {
        pPrev->right = GetNodeOffset(pNewNode);
    }

    sem_post(&mSemaphore);
    return 0;
}

int SharedBinaryTree::OpenMapFile(const string& filename, size_t size, bool create)
{
    if (create) {
        unlink(filename.c_str());
    }

    int fd = open(filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        SPR_LOGE("Open %s fail! (%s)\n", filename.c_str(), strerror(errno));
        return -1;
    }

    if (ftruncate(fd, size) == -1) {
        SPR_LOGE("ftruncate fail! (%s)\n", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

Node* SharedBinaryTree::CreateNode(const string& key, const string& value)
{
    // mCurUsedSize was already incremented by sizeof(Node) in SetValue(),
    // so the new node starts at mCurUsedSize - sizeof(Node).
    Node* newNode = (Node*)((char*)mRoot + mCurUsedSize - sizeof(Node));

    strncpy(newNode->key, key.c_str(), SHARED_BTREE_KEY_MAX_LEN - 1);
    newNode->key[SHARED_BTREE_KEY_MAX_LEN - 1] = '\0';
    strncpy(newNode->value, value.c_str(), SHARED_BTREE_VALUE_MAX_LEN - 1);
    newNode->value[SHARED_BTREE_VALUE_MAX_LEN - 1] = '\0';

    // 新节点的左右子节点初始化为0（空）
    newNode->left = 0;
    newNode->right = 0;

    return newNode;
}

void SharedBinaryTree::GetAllKeyValues(std::map<std::string, std::string>& keyValueMap)
{
    sem_wait(&mSemaphore);
    // 从根节点开始遍历，包括根节点本身
    GetKeyValue(mFirstNode, keyValueMap);
    sem_post(&mSemaphore);
}

void SharedBinaryTree::GetKeyValue(Node* pNode, std::map<std::string, std::string>& keyValueMap)
{
    if (pNode != nullptr && pNode->key[0] != '\0') {
        // 先遍历左子树
        GetKeyValue(GetNodeByOffset(pNode->left), keyValueMap);
        // 再添加当前节点
        keyValueMap[pNode->key] = pNode->value;
        // 最后遍历右子树
        GetKeyValue(GetNodeByOffset(pNode->right), keyValueMap);
    }
}
