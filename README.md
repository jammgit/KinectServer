# Kinect Server
## 简述
[![](icon.png)](https://github.com/jammgit/KinectServer)

> 这是一个`网络化Kinect`设备数据的工程,属服务端开发。

## 开发环境
* Visual Studio 2015
* QT5.7.0
* Win7 64bit

## 相关库
* [Boost](http://www.boost.org/) - C++ Libraries
* [Kinect SDK for Window](http://www.itellyou.cn/) - Window Kinect develop kits
* [libyuv](https://github.com/lemenkov/libyuv) - libyuv is an open source project that includes YUV conversion and scaling functionality.
* [openh264](http://www.openh264.org/) - Made by Csico, free h264 encode libraries
* [protobuf](https://github.com/google/protobuf/) - Protocol Buffers - Google's data interchange format

## 模块简介

| 模块名       				 |  功能  |
| --------:  				 | :---- |
| IKSKinectDataCapture       | 负责捕获Kinect设备数据	|
| IKSKinectDataEncode        | 负责对Kinect数据进行变换与编码   |
| KSKinectDataServer         | Kinect数据服务器    |
| KSKinectService        	 | KinectServer控制指令服务器    |
| KSKinectDataService        | 解析Kinect数据请求    |
| KSLogService        		 | 日志打印    |
| KSUtils        			 | 工具集   |
| KinectServer        		 | Qt主工程  |


## 联系作者
* QQ:269337654
* Email:269337654@qq.com
* Blog : [依然那霖哥](http://blog.csdn.net/jammg “依然那霖哥”)


## 客户端Git
[https://github.com/joshion/3DShow](https://github.com/joshion/3DShow)