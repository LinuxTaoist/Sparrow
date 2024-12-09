# Ubuntu tmux 修改默认前缀键

默认情况下，tmux的前缀键是Ctrl+b，可以通过修改配置文件来修改它。

1. 打开tmux配置文件：

```bash
vim ~/.tmux.conf
```

2. 在配置文件中写入以下内容：

```shell
set-option -g prefix F1
unbind-key C-b           # 取消默认的前缀键绑定
bind-key F1 send-prefix  # 设置新的前缀键
```

3. 重新加载tmux配置文件：
```bash
tmux source-file ~/.tmux.conf
```
