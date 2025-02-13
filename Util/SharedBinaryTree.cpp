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

SharedBinaryTree::SharedBinaryTree(const string& filename, size_t size)
    : mRoot(nullptr), mSize(size), mCurUsedSize(0), mFirstNode(nullptr), mFilename(filename)
{
    mHandler = OpenMapFile(filename, size);
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
    mCurUsedSize = sizeof(mCurUsedSize);

    // init first node
    mFirstNode = (Node*)((char*)mRoot + mCurUsedSize);
    if (mFirstNode != nullptr) {
        mFirstNode->key[0] = '\0';
    } else {
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

    if (remove(mFilename.c_str()) == -1) {
        SPR_LOGE("remove fail! (%s)\n", strerror(errno));
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
            pNode = pNode->left;
        } else {
            pNode = pNode->right;
        }
    }

    sem_post(&mSemaphore);
    return ret;
}

int SharedBinaryTree::SetValue(const string& key, const string& value)
{
    sem_wait(&mSemaphore);

    if (key.size() >= KEY_SIZE ||  value.size() >= VALUE_SIZE) {
        SPR_LOGW("Lenght out of limit! Limit length: %d %d\n", KEY_SIZE, VALUE_SIZE);
    }

    Node* pNode = mFirstNode;
    if (pNode == nullptr) {
        SPR_LOGE("pNode is nullptr! \n");
        sem_post(&mSemaphore);
        return -1;
    }

    Node* pPrev = nullptr;
    while (pNode != nullptr) {
        if (key == pNode->key) {
            strncpy(pNode->value, value.c_str(), VALUE_SIZE);
            pNode->value[VALUE_SIZE - 1] = '\0';
            sem_post(&mSemaphore);
            return 0;
        }

        pPrev = pNode;
        pNode = (key < pNode->key) ? pNode->left : pNode->right;
    }

    // 更新共享内存使用大小
    mCurUsedSize += sizeof(Node);
    if (mCurUsedSize > mSize) {
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

    *reinterpret_cast<size_t*>(mRoot) = mCurUsedSize;

    if (key < pPrev->key) {
        pPrev->left = pNewNode;
    } else {
        pPrev->right = pNewNode;
    }

    sem_post(&mSemaphore);
    return 0;
}

int SharedBinaryTree::OpenMapFile(const string& filename, size_t size)
{
    unlink(filename.c_str());
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
    Node* newNode = (Node*)((char*)mRoot + mCurUsedSize);

    strncpy(newNode->key, key.c_str(), KEY_SIZE - 1);
    newNode->key[KEY_SIZE - 1] = '\0';
    strncpy(newNode->value, value.c_str(), VALUE_SIZE - 1);
    newNode->value[VALUE_SIZE - 1] = '\0';

    newNode->left = nullptr;
    newNode->right = nullptr;

    return newNode;
}

void SharedBinaryTree::GetAllKeyValues(std::map<std::string, std::string>& keyValueMap)
{
    sem_wait(&mSemaphore);
    GetKeyValue(mFirstNode->left, keyValueMap);
    GetKeyValue(mFirstNode->right, keyValueMap);
    sem_post(&mSemaphore);
}

void SharedBinaryTree::GetKeyValue(Node* pNode, std::map<std::string, std::string>& keyValueMap)
{
    if (pNode != nullptr) {
        GetKeyValue(pNode->left, keyValueMap);
        GetKeyValue(pNode->right, keyValueMap);
        keyValueMap[pNode->key] = pNode->value;
    }
}