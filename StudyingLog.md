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





---



### P3_GLEW_UseModernGL

如何访问显卡提供给咱们的API接口呢？当然，在Window平台，可以用Win32接口获取，但是数量太多了，而且版本不同。所以需要一些库进行获取相应的函数指针，这些库叫做glew、glue、glad等



引入glew的几个注意点：

（1）头文件包含的顺序，必须要glfw之前

```c++
#include <GL/glew.h>
#include <GLFW/glfw3.h>
```

（2）glew如果采用静态库链接，必须添加宏GLEW_STATIC

（3）glewInit接口若要调用成功，一定要已经存在Windows上下文！





### P4_VBO_VertexAttribute

介绍现代OpenGL的一些概念：顶点缓冲区（VBO）、VertexAttribute、DrawCall、状态机等概念

利用glDrawArrays绘制一个最简单的三角形！







### P5_Shader

上节我们没有写任何Shader也能绘制一个三角形，其实是驱动程序给的默认Shader。

Shader其实就是跑在GPU上的一段程序，同样需要编译、链接。常见的着色器分为：顶点着色器、片段着色器！

后面学习高级部分还有：几何着色器、细分着色器等等！



顶点着色器，针对每一个Vertex调用一次。片段着色器，针对每一个像素调用一次！





### P8_ShaderOpt

1、Shader代码放到文件中，进行读取获取

2、添加res/shaders资源文件夹

咱们将vertexshader和fragmentshader放到同一个文本中，通过行标识进行切换





### P9_IndexBuffer

1、介绍什么叫做索引缓冲区EBO、IBO

2、它解决了什么问题？答：顶点重复存储

![image-20240617123801939](StudyingLog.assets/image-20240617123801939.png)



### P10_Deal_Error

1、不引入外部工具，利用OpenGL提供的接口，封装接口，获取错误说明

2、两种方法：

```
(1) gl get_error  1.1 我们用这个
(2) message_call_back  4.3以上
```

3、宏封装，防止污染代码。且提供断点。

4、提示错误在哪个地点



### P11_Uniform

什么是uniform变量，其实就是cpu向gpu传的变量数据，在Shader中接收





### P12_VAO

启动OpenGL核心模式，而不是兼容模式（开启VAO绑定）

VAO解决了什么问题，有什么好处



### P13_AbstractOpenGL

简易使用OOP封装OpenGL中的许多概念：VertexBuffer、VertexArray、IndexBuffer、VertexBufferLayout等



### P14_AbstractShader

抽象Shader相关



### P16_Renderer

抽象渲染类，将剩余的代码进行封装起来



### P17_Texture

引入纹理

引入stb_image库读取图片

TODO：混合的引入，好像没有解决背景透明的显示



### P18_Blend

如果要渲染有色玻璃、半透明物体。怎么做呢，开启混合

默认OpenGL不开启混合

如何控制呢？

```
glEnable(GL_BLEND)-glDisanble(GL_BLEND)
glBlendFunc(src, dest)
===> src default is GL_ONE
===> dest default is GL_ZERO

glBlendEquation(mode)
===> default is GL_ADD -> src + dest

so default: color == 1 * srcColor + 0 * destColor
```



### P19_Math

数学的作用

向量主要分两类：方向向量和位置向量

引入OpenGL配套的glm数学库



投影矩阵的作用：



### P22_Inter_ImGui

了解实时UI框架imgui，并进行集成！



### P23_Batch_Render

如何批量渲染多个物体，承接上节，针对同一的VAO和VBO，修改uniform，调用两次DrawCall实现两次渲染



### P24_TestFramework

封装一个沙盒测试框架，用于各个测试用例的快速切换和测试的添加



### P24_TestFramework

移植之前的纹理的测试用例



### P27_BatchRendering

如何批量渲染大量形状的简易二维几何物体

批渲染：所有几何图形集中到一个独立的顶点缓冲区和索引缓冲区，就好像一个几何体，然后调用一次DrawCall



批渲染和传统渲染有何不同，两者的优缺点对比



让我们带着几个问题切入：

（1）如何绘制任意数量的不同颜色的四边形

（2）如何绘制任意数量的不同纹理的四边形

```
思考：如果纹理单元不够用，怎么处理？
答：分组批渲染

//注意这里的坑
m_Texture1 = std::make_unique<Texture>("res/textures/logo.jpg");
m_Texture2 = std::make_unique<Texture>("res/textures/logo2.png"); //注意这里，必须先构造完毕再同时绑定。构造绑定+构造绑定是行不通的
m_Texture1->Bind(0);
m_Texture2->Bind(1);
```

（3）如何绘制任意数量的动态几何体

```
思路：VBO先开辟一块大的内存区域，先不给数据，后面通过调用接口，每次渲染前给数据

给VBO预开辟的内存写数据：
两种方法：
法1：
        //glMapBuffer();
        ////write content
        //glUnmapBuffer();

法2：
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(qs), qs);
```



考虑如何扩展IBO，适应批量渲染？

```
1、确定最大渲染四边形数量
2、确定最大顶点数量
3、确定最大索引数量
```







