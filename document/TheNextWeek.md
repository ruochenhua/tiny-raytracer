# 《Ray Tracing：the Next Week》
>百尺竿头，更进一步

[Ray Tracing: the Next Week](https://raytracing.github.io/books/RayTracingTheNextWeek.html)是系列的第二本书，也是光线追踪的进阶教程。读完这本书，已经可以做出一个“真正”的光线追踪器了，虽然肯定还比不上专业的光线追踪器，但是作为一个业余的兴趣工程来说，已经能够自娱自乐。

这次学习的经验和想法，我便在此处整理一番。

## BVH(Bounding Volume Hierarchies)
光线追踪的很多内容其实和之前做物理碰撞的内容是非常相近，优化的第一步往往都是从**构建管理object的结数据构，用粗略但快速的算法过滤掉大多数不需要处理的object，用精准但耗时的算法去处理剩下的少数object**。

这种数据结构有*四叉树、八叉树、BVH树、BSP树、k-d树*等等，这里教程主要用的是BVH树，当然这些算法的理念都很相近，根据实际情况选最优的算法来处理。

![BVH示意图](https://raytracing.github.io/images/fig-2.01-bvol-hierarchy.jpg)

上面是教程的BVH示意图。
简单来说就是将场景中的所有object进行以下处理
1. 将所有object用一个大包围体（Bounding Volume）包起来，作为树状结构的父节点

2. 从这个父节点开始，将里面的所有object根据**规则**分成两个集团，每个集团重新构建一个包围体包围所有的object，作为这个父节点的子节点。

3. 对每个子节点递归进行上一步操作，直到无法分出更小的集团

第二步的**规则**可以是根据包围体内的所有object在某个轴的位置进行二分（轴的选取可以随机，但选择object跨度最大的效果更好），当然最好确保二叉树的平衡。

这样遍历场景中所有object的时间复杂度从O(n)降为Olog(n),对object数量多的场景帮助很显著。

为了实现BVH数结构，每个object需要有对应的Bounding volume，theNextWeek里面选择的是AABB，它一般来说是会比球体效果更好的。另外也有OBB这种可以作为Bounding volume，可能要看实际场景选择。

至此，可以在光线追踪进行的时候，先进行光线和包围体的碰撞检测（快速），且若没命中父节点，子节点肯定也不会命中，节省了很多时间。

## 关于Texture Mapping
theNextWeek里面介绍了球体映射贴图的一种实现。其实也就是根据球的纬度映射到贴图的v，用球体的经度映射到贴图的u，并且uv的范围是[0,1]。

## 关于Perlin Noise
Perlin Noise是一个挺有意思的议题。后面如果有时间我想专门研究一下它的实现以及各种变种。不过这里我不是很想深入探讨，这里的Perlin Noise其实就是作为一种贴图的效果呈现，感觉和光线追踪这个话题的相关性其实并不深，不过下面还是贴几张教程中的Perlin Noise的效果图好了。
![Perlin texture with trilinear interpolation](https://raytracing.github.io/images/img-2.10-perlin-trilerp.png)
![Perlin texture, shifted off integer values](https://raytracing.github.io/images/img-2.13-perlin-shift.png)
![Perlin noise, marbled texture](https://raytracing.github.io/images/img-2.15-perlin-marble.png)


## 关于四边形
之前光线追踪器只支持球体，因为球体确实是数学表现上最简单的一种形状了。
这次新增了四边形，其表现形式是通过在四边形上的一个角Q，加上另外两条边u，v。
![四边形](https://raytracing.github.io/images/fig-2.05-quad-def.jpg)

光线和四边形的碰撞检测，分为两步：
1. 光线和四边形所处平面的相交点P计算，剔除掉相交点在射线方向后方，以及光线和平面平行的情况。
2. 相交点P投影到以u，v为坐标轴，以Q为坐标原点的2D坐标系统上，判断P的xy坐标是否在[0,1]的范围之内。


