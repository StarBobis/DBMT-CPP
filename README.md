# DBMT

这里是旧的DBMT的代码，也是最后一次更新C++版本的DBMT，我打算还是把它开源了，也许能提供一些参考价值。

开发DBMT的过程是一个不断摸索的过程，一开始是用Python写的3Dmigoto-Tailor，但是由于高度绑定PyCharm Community，用起来很不方便，
且执行速度也很慢，提取需要一分钟，生成Mod需要一分钟。尤其是TANGENT值重新计算，使用Python做的话，一个轮廓线算法就能执行2-10分钟。
特别是顶点数量上去之后，后处理所需的时间大幅度增加，所以我使用C++开发了3Dmigoto-Wheel，
一开始它是通过参数运行的，在黑框框里，但是太难用了，随后我把参数放到了json文件里，然后每次运行只需要改写json配置文件，
但是这仍然很不方便，于是我使用tkinter做了第一个GUI界面，来写入json传递参数。
用了一段时间发现界面太丑了，更高级的功能无法实现，比如多个IB同时提取的需求，以及每个IB单独的配置项。
所以又使用WinForm重做了界面，这下方便了，但是刚开始开发的时候功能比较少，所以简单的界面足以满足需求，

当功能不断增多时，当支持的游戏不断增多时，当配置不断增多时，使用C++作为核心的缺点彻底暴露了出来：

1.由于经验不足，边开发边摸索逻辑，导致了不完全的面向对象和不完全的面向函数编程，导致逻辑不断增多后，代码变成屎山。

2.由于屎山难以维护，我尽可能的把一些任务抽象成类来处理，这期间完全重构了4-5次代码，但是随着新需求和新理解的不断出现，旧的代码仍然需要不断重构，
导致项目到现在彻底变成了屎山。

3.编译时间过于长，尽管我已经把代码分为了多个lib文件的形式，但是每次改动一个最核心的GlobalConfig类，所有依赖于它的代码文件都要进行一次完全编译，
这导致即使拆分了多个lib项目，最终的编译时间仍然达到了1分钟以上，即使是只改变一个小的逻辑，编译也需要5-10秒，由于架构是读取json文件，
而json文件又是由GUI进行操作生成，这导致每个微小的改动，哪怕是改一个输出的日志提示的几个字，都需要1分钟左右的时间来进行部署，
最后才能去GUI里进行测试，然后测试又需要时间，导致大量时间浪费在编译=>部署=>测试的循环上，有些功能甚至测了上百次。

4.WinForm编写的GUI，C++编写的核心，Python编写的Blender插件，其语言跨度过大，之前交给踩蘑菇的三位专业程序员，每个人负责一个对应的部分，
到最后两个多月的时间，没有一个人进行任何开发任务，让一个人同时熟练使用3种语言是很难的，让3个不同语言的人共同开发一个软件，又有沟通成本的问题，
更何况DBMT是强业务依赖型软件，必须3个开发者都熟练使用3Dmigoto，才能进行合作开发。

总之：跨3种语言的架构让其它人加入开发的难度变得超高，在MMT开源的半年内没有任何人参与开发，甚至没有人能成功编译项目（boost库拦住了一部分）。

5.TANGENT应该在Blender中重新计算而不是在导出后进行后处理，在Blender中点击就应该直接生成Mod并在游戏中F10刷新查看，
GUI与核心C++程序不应该通过复杂的Json文件进行调用而是应该通过编写为dll进行调用，这是1.5年的摸索经验得出的最终结论：现有的架构并不是最大化方便Mod制作的最佳实践。

6.截至此版本发布，待办列表里积累了将近一百个新的需求，有一半以上需要新架构开发，这意味着要么我重构C++代码，
要么继续忍受每次改动都要经历3分钟的 编译->部署->测试流程，随后跨3种语言进行功能开发，要么在旧的架构里直接加《立即性》代码来实现功能，继续写屎山。

7.WinForm太过于古董，以至于界面已无底成本继续优化的可能性，许多新的需求依赖的界面设计在WinForm中无法实现。

8.其它Mod工具开发者全部使用Python语言，如果要集成他们的功能，那么需要再额外人力转换为C++语言。

还有很多很多无法列举的缺点，那么如何解决这些问题呢？答案是使用Python

Python + PyQt5 + PyFluentWidgets

- 新架构使用VSCode更加轻量级，把Blender插件，核心程序，GUI界面集中在一个项目里开发，且使用一种语言Python就能搞定。
- PyFluentWidgets实现更好的界面，直接用PyInstaller就能打包成exe方便使用，任何功能测试时都无需重新编译部署。
- Python写的核心可由Blender插件直接以模块的方式调用，在Blender里生成Mod后游戏里直接刷新查看。
- 快速集成其它Mod工具作者开发的新功能和代码。
- Python学习成本低，会有更多人加入开发，不再是一个人的游戏了。
- 完全开源，避免打包的exe被怀疑藏毒的drama。
- 将全部使用面向对象编程来避免屎山问题，一切皆是对象(跟WWMI学的)


# 面向未来

总之，C++版的工具我是花了1.5年时间尝试过了，但是很明显它并不符合实际Mod制作需求，最终即使舍不得也不得不放弃了。

新架构DBMT:https://github.com/StarBobis/DBMT  欢迎加入开发。