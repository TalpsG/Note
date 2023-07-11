# blog

## html

### 基础语法

head 是指 标签页上面
body：页面内能看到的
p 段落
div 分组元素
h1 一级标题 最多 6 级
a 链接

#### box model

1. margin
2. border
3. padding
4. content

## css

### 基础语法

```css
div {
  color: red;
  key: value;
}
```

css 要在 html 中引用才可以
`<link ref="stylesheet" href="style.css" />`

下面的例子中，div 标签内容变为红色

```html
<div>我是红色</div>
```

```css
div {
  color: red;
}
```

也可指定标签的类，使得所有类为 info 的标签内容都变为红色,类前要加一个 dot

```html
<div class="info">我是红色</div>
```

```css
.info {
  color: red;
}
```

也可以指定 id,id 前加#

```html
<div id="info">我是红色</div>
```

```css
#info {
  color: red;
}
```

**id 是唯一的，一个 id 值只能给一个标签，但是一个标签可以有多个 class,class="a b c"**

#### 变量

下面代码中 root 为一个变量集合，内部为变量以及对应的值

```css
:root {
  --v1: value1;
  --v2: value2;
}
```

调用变量
`attribute1:var(--v1)`

#### 多值的属性

比如 margin 可以有多个值,第一个为上的边缘宽度，然后后面的是顺时针转动的，第二个是右侧，然后是下方，然后是左侧
`margin:10px 20px;`

#### 8 grid system

padding 填充 box

#### border radius

可以使正方形图片圆形显示

#### max-width

当窗口的大小大于 max-width 就按照 max-width，小于 max-width 就按照窗口大小

#### display:

block:该标签的展示是块展示，同样的还有 inline 的展示
flex:横着展示

#### flex

flex grow 如果有横向空闲区域则按照 flex-grow 的比值去分配空闲区域到每一个 box
flex-basis:flex 元素的大小

#### hr

background-color //hr 颜色
height //hr 粗细
border // 根据实际样式看看是否需要 border 不需要设置 none

#### +号

```css
img + p {
  font-weight: bold;
}
```

## js 语法

### let var

var 声明的是全局变量，let 是局部变量

### 结构体

```javascript
let a = {
  attribute: value,
};
var a = {
  attribute: value,
};
```

### 函数

```javascript
function func1(para1, para2) {
  console.log(1);
}
let fun2 = function (paras) {};
let fun3 = (paras) => {};
```

### class

```javascript
class student {
  constructor() {}
}
```

### == ===

等于与严格等于，严格等于表示变量类型相同且值相同，等于表示语义相同即可

### 循环

```javascript
for (let a of b) {
}
```

### 复制元素

复制数组用[] 复制结构体用{}

```javascript
let a = [1, 2, 3];
let b = [...a]; //拷贝
let c = a; //仅仅是指向
```

### prompt

弹窗输入

### 输出

consolelog 或 alert
如果想在页面上显示可以使用
`document.getElementbyId('标签的id').innerHTML = contents`

### getElementBy

得到的是字符串元素，需要调用 parse 解析值

### 绑定事件

onclick
onchange
onmouseover
onmouseout
onkeydown

```html
<div onclick="MyClick()">123</div>
```

点击 123 就会执行 MyClick 函数。

### setinterval
实现每隔一段时间执行某个函数

## py 语法

### is 和 ==

==是值的比较 而 is 是对象的 id 作比较

### 引用式变量
变量名是变量对象的一个指针，同理list tuple等内部也是存放的各个元素的指针。
元组的内部元素的引用是不可改变的，但是引用指向的元素是可以改变的，比如元组中的list可以添加或者删除

### 函数参数
当函数参数为可变类型的时候，实参是可能被修改的，但是如果是不可变类型则传参类似于c语言
