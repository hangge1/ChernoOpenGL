OpenGL 通过调用显卡，强有力绘制图形。



OpenGL不是框架、引擎吗，它是一套规范，规范了API，本身是不涉及任何实现！

OpenGL不是开源的，是跨平台的。



**那么谁负责实现OpenGL API 接口呢？**

答：显卡生产商。





## 开始编码

### P2_CreateWindow

利用glfw进行窗口管理，创建上下文，IO事件分发等！

只需要用pre-build二进制库就可以了，咱们不需要关注glfw内部的源码！

（1）调整输出目录和中间目录路径

（2）引入glfw库的头文件和库路径引用

（3）使用legacy opengl接口，绘制一个三角形



结果如下：

![image-20240616152520386](./StudyingLog.assets/image-20240616152520386.png)