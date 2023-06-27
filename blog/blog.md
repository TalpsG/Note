# blog
## html
### 基础语法
head 是指 标签页上面
body：页面内能看到的
p 段落
div 分组元素
h1 一级标题 最多6级
a 链接

## css 
### 基础语法
```css
div{
  color:red;
  key:value;
}
```
css要在html中引用才可以
`<link ref="stylesheet" href="style.css" />`

下面的例子中，div标签内容变为红色
```html
<div>我是红色</div>
```
```css
div{
  color:red;
}
```
也可指定标签的类，使得所有类为info的标签内容都变为红色,类前要加一个dot
```html
<div class="info">我是红色</div>
```
```css
.info{
  color:red;
}
```
也可以指定id,id前加#
```html
<div id="info">我是红色</div>
```
```css
#info{
  color:red;
}
```
