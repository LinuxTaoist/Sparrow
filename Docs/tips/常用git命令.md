# 常用git命令

## 基础命令
1. 初始化和克隆：
    - git init：初始化一个新的 Git 仓库。
    - git clone \<url>：克隆远程仓库到本地。
2. 查看状态和日志：
    - git status：显示工作目录和暂存区的状态。
    - git log：查看历史提交记录。
    - git reflog：查看所有对 HEAD 的更新，包括那些不会出现在 git log 中的操作。
3. 添加和提交更改：
    - git add \<file> 或 git add .：将文件添加到暂存区。
    - git commit -m "message"：提交暂存区的更改，并附上一条提交信息。
## 分支管理
1. 创建、切换和合并分支：
    - git branch：列出所有分支。
    - git branch \<branch>：创建新分支。
    - git checkout \<branch> 或 git switch \<branch>：切换到指定分支（switch 是 Git 2.23 版本引入的新命令）。
    - git checkout -b \<branch> 或 git switch -c \<branch>：创建并切换到新分支。
    - git merge \<branch>：合并指定分支到当前分支。
    - git rebase \<branch>：将当前分支的更改应用到指定分支之后。
## 远程仓库交互
1. 获取和推送更新：
    - git fetch：从远程仓库下载数据但不自动合并或修改当前工作。
    - git pull：从远程仓库获取最新更改并自动合并到当前分支。
    - git push：将本地提交推送到远程仓库。
## 检查和比较
1. 差异检查：
    - git diff：显示已修改但尚未暂存的文件更改。
    - git show：显示某次提交的具体信息。
## 撤销操作
1. 撤销更改：
    - git reset：回退版本到指定状态，可以是软回退（保留工作目录更改）或硬回退（放弃工作目录更改）。
    - git revert \<commit>：创建新的提交以撤销指定的提交。
    - git clean：删除未追踪的文件。

## 常见场景
1. 恢复未推送的本地提交：
```shell
git reflog
git checkout <commit-hash>
git branch <new-branch-name> # 可选。 找到目标提交，可创建新分支保存。
```

2. 本地清理已删除的远程分支：
```shell
git fetch --prune
```

3. 添加提交模板：
```shell
git config --global commit.template <gitmessage.txt>
```

4. 查看分支：
```shell
git branch -r   # 查看远程分支
git branch -avv # 查看本地和远程分支
```

5. 删除分支：
```shell
git push origin --delete <branch-name> # 删除远程分支
git branch -d <branch-name>  # 删除本地分支
```

6. 查看远程仓库地址：
```shell
git remote -v
```

7. 本地代码提交冲突解决：
```shell
git reset --soft HEAD^ # 撤销最近一次提交，保留更改
git stash              # 保存更改
git pull               # 更新本地代码
git stash pop          # 恢复更改
...                    # 解决冲突
git add .              # 添加更改
git commit             # 提交更改，备注提交信息
git push               # 推送更改
```