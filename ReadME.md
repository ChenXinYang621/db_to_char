## 参数问题

MySQL 的参数输入转化到 mysql.h 是跟注释不一样的

> 带小数点会转化为 Real_Result
>
> 不带小数点会转化为 Decimal_Result

## 一个参数的情况
> ### 校验问题

一个参数基本上就是数字和字符串，字符串不做约束。如果是数字，那么，第一个字符只有 '+', '-', '.', '[0-9]' 这几种情况

> ### 实现问题

实现问题只需考虑在非 string 情况下正号和多于的 0 需要去除的问题

## 运行

```bash
# -I 指定目录搜索头文件
# -o 指定输出文件格式和地址
# -shared 表示动态库
g++ -I "C:\Program Files\MySQL\MySQL Server 8.0\include"  -o "C:\Program Files\MySQL\MySQL Server 8.0\lib\plugin\to_char.dll" -shared -fpic  .\main.cpp

# ubuntu 编译
g++ -I /usr/include/mysql  -o /usr/lib/mysql/plugin/to_char.so -shared -fpic  1.cpp
```

某个傻逼错误困扰了我半天

## 感悟

原本其实想按照格式在 init 函数判断，但是被之前的思路带偏了。现在使用 convert() 函数进行转化。之前把太多的时间花在了 MySQL 的格式检查方面，没考虑到 MySQL 本身对格式有一定形式的检查