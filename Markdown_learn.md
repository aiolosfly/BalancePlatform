This is an H1
===

This is an H2
---

***
列表
* 1
* 2
* 3
* 4
***

* 1
    * 2
    * 3
    * 4

***
+ 1
+ 2
+ 3
+ 4

***
- 1
- 2
- 3
- 4

***
1. 1
2. 2
3. 3
4. 2

***
选择列表
- [ ] yes
- [x] no

这是小写字母“x”

***

> 这里是引用，
可以试一下

> 引用
> 不换行

> 引用
>
> 引用换行了

>> 引用中的引用 

注意符号和文本之间的空格

***

#### 插入链接
（注意符号和文本之间的空格）

## [Baidu](http://baidu.com)

或者

## [Baidu] [1]

 [1]: http://baidu.com/
 

This is [an example](http://baidu.com) inline link.

 [This link](http://baidu.com) has no title attribute.


#### 插入图片

![baidu](https://ss0.bdstatic.com/5aV1bjqh_Q23odCf/static/superman/img/logo/bd_logo1_31bdc765.png)

插入图片首先要有储存图片的地方（图床）生成URL，存放在github也行

#### 插入邮箱
<1656219949@qq.com>

***

*这是斜体*

**这是粗体**

***这是又粗又斜***

## ***这是又粗又斜又大***

++这是下划线++

~~这是删除线~~

***

header 1 | header 2|header 3|
|---|---|---|
row 1 col 1 | row 1 col 2|row 1 col 3|
row 2 col 1 | row 2 col 2|row 2 col 3|


| Tables        | Are           | Cool  |
| ------------- |:-------------:| -----:|
| col 3 is      | right-aligned | $1600 |
| col 2 is      | centered      |   $12 |
| zebra stripes | are neat      |    $1 |

注意上的冒号“：”。默认是靠左；居中，靠右。

***

# `code`

`这是优雅而浅浅的代码框,但是换不了行`

```
用这个随时可以换行
不行你试试
而且是在同一个代码框内的
```

注意“``”是跟键盘上波浪号同框的，不是单引号

```ruby
require 'redcarpet'
markdown = Redcarpet.new("Hello World!")
puts require markdown.to_html
require
```

```c
#include <stdio>

int main()
{
    if(1>2)
        print("hello world");
    
    return 0 ;

}
```
指定可选语言标识符，可以为该语言关键词标色

***

\*用反斜线消除符号\*

***

先不解释
*   [概述](#overview)
    *   [哲學](#philosophy)
    *   [行內 HTML](#html)
    *   [特殊字元自動轉換](#autoescape)


<h2 id="overview">概述</h2>

<h3 id="philosophy">哲學</h3>

实现页内跳转

***

[流程图语法](http://knsv.github.io/mermaid/#flowcharts-basic-syntax)



```
graph TB
    A[rectangle]-->B(round edges)
    A-->|text|C{rhombus}
    B-->|arrow|D((circle))
    C---|link|E>asymetric]
    A--text-->F[rectangle]
    
    F-.->G[rectangle]
    F-.->|Dotted link|H((circle))
    
    D==>|thick line|I[hhh]

    C-->J
```
```
graph BT
    A[rectangle]-->B(round edges)
    A-->|text|C{rhombus}
    B-->|arrow|D((circle))
    C---|link|E>asymetric]
    A--text-->F[rectangle]
    
    F-.->G[rectangle]
    F-.->|Dotted link|H((circle))
    
    D==>|thick line|I[hhh]

    C-->J
```
```
graph RL
    A[rectangle]-->B(round edges)
    A-->|text|C{rhombus}
    B-->|arrow|D((circle))
    C---|link|E>asymetric]
    A--text-->F[rectangle]
    
    F-.->G[rectangle]
    F-.->|Dotted link|H((circle))
    
    D==>|thick line|I[hhh]

    C-->J   

```
***
[有道云markdown示例](http://note.youdao.com/iyoudao/?p=1895)

***
数学LaTex

`$Y = X^2$`

`$Y = X_2$`

```math

\oint_C (x^3\,dx + 4y^2\,dy)

\sum_{i = 0}^{n}i^e

```









