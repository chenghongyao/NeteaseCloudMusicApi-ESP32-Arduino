# 仓库说明
本项目是基于 ESP32 和 Arduino 框架的网易云 api，使用 PlatformIO 编写。

项目主要参考自[NeteaseCloudMusicApi](https://github.com/Binaryify/NeteaseCloudMusicApi)、[ncmbot](https://github.com/xiyouMc/ncmbot)和[DecryptLogin](https://github.com/CharlesPikachu/DecryptLogin)。其中ncmbot是可能是较早较完善的api库，基于python2，是我早期玩网易云最快乐的源泉，目前似乎已经停止更新，[NeteaseCloudMusicApi](https://github.com/Binaryify/NeteaseCloudMusicApi)是目前最完善的，基于nodejs，本项目的大部分接口都参考自这个项目。


正在持续更新测试中，后期可能会改为独立的Arduino库。

# 依赖库
- [ArduinoJson 6.17.3](https://github.com/bblanchon/ArduinoJson)

# 已完成
> 详细请看netease.hpp文件

- `login`：登录
- `getRecommendSongs`：获取每日推荐歌曲
- `getUserPlaylists`：获取用户所有歌单(包括创建和收藏)
- `getPlaylistDetail`：获取歌单信息
- `getUserDetail`：获取用户信息
- `getUserAccount`：获取当前登录的账户信息
- `getUserLevel`：获取当前登录的用户等级
- `getMusicUrl`：获取歌曲的url信息（下载链接，只对免费歌曲有效）
- `getUserRecord`：获取用户的听歌排行
- `getUserFollows`: 获取用户关注
- `getUserFolloweds`：获取用户粉丝
- ~~`getUserEvent`: 获取用户动态(返回的JSON太大，无法设置返回长度)~~
- `getPersonalFM`: 获取私人FM(每次请求返回3首歌的信息)

# 使用示例
目前看看 main.cpp 中的示例
