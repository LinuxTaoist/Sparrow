# 第三方库集成

1. 在3rdParty目录下创建对应第三方路径，例 `3rdParty/googletest`
2. 提供对外头文件，放置对应Include目录下，例 `3rdParty/googletest/include/`
3. 提供对外库文件，放置对应Lib/{platform}目录下，例 `3rdParty/googletest/lib/Default/`
4. 提供对应源码，压缩为tag.gz_{version}格式，放置第三方目录下，例 `3rdParty/googletest_v1.3.0.tag.gz`
