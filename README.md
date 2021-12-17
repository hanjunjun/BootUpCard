# 1 远程开机卡
## 1.1 实现的功能列表
>1.通过阿里云服务器TCP服务发送命令给单片机，单片机收到消息之后给台式机主板发送关机/开机命令
>
>2.没有做APP端给单片机配网功能，现在是直接写死的阿里云服务器地址和连接的wifi账号密码

## 1.2 加群交流
>群号码：963718093 开源交流群，加群一起学习共同进步。
[![.Net Core 开源学习交流](http://pub.idqqimg.com/wpa/images/group.png ".Net Core 开源学习交流")](http://shang.qq.com/wpa/qunwpa?idkey=d42b97a72adbb99729c59fc68173df53093e6d8908dd4588f2d81907a84d8f3b)

## 1.3 完整场景
>```
>1.手机给单片机配网，设置单片机连接wifi和连接公网TCP服务器
>2.用户操作APP调用web服务接口，web服务下发控制命令给单片机实现开关机和状态监控
>```

##  1.4 Demo演示
![步骤一](https://raw.githubusercontent.com/HanJunJun/BugReport/master/Document/images/122.png)
