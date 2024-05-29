# lec2 sql

sql包含了三种语言的功能
1. dml 数据操作语言
2. ddl 数据定义语言
3. dcl 数据控制语言 访问权限等


## 语法
基本语法:
`select name ,gpa from students where age >25;`

## 聚集函数
一些常用函数，比如`count`,`max`,`min`等。
`select COUNT(*) as cnt from student where login LIKE "%@cs" `

`COUNT AVG SUM`支持`DISTINCT`会先去重，在进行函数运算。

`GROUP BY`:
`select AVG(s.gpa),e.cid 
from enrolled AS e,student AS s
where e.sid = s.sid
GROUP BY e.cid`
上述语句会按照`e.cid`进行分组，之后运算平均值，然后输出每一个组的平均值。

`HAVING`：
在`GROUP BY`后查询的结果，想要继续处理要使用`HAVING`

`CREATE TABLE tname (select )`

`LIMIT`：限制输出的行数


`ALL ANY IN EXISTS`:
`ALL`是必须每一行都相同
`ANY`有一行就行，等价于`IN`
`EXISTS`只要查询有结果就为真
`selecto from  where a = ALL(inner query)`



## 结论
可以执行复杂的sql语句，让数据库系统自动优化。

## sql hw
字符串拼接: `column1 || "asdas" as new_column`
对某个列的`NULL`或其他特殊值进行特殊处理: `CASE when column ISNULL THEN 2023 else column END`

`CTE`：可以生成公共表达式，供下面的查找语句使用。
`DENSE_RANK`和`ROW_NUMBER` 都可以实现查找某个类别的固定个数的元素。

`Inner Join`