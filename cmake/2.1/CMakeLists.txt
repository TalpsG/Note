cmake_minimum_required(VERSION 3.20)

# set设置单个变量值
set(temp1 talps)
message(${temp1})

# 设置多个值
set(listvalue a1;a2)
message(${listvalue})

# 环境变量

message($ENV{PATH})
set(ENV{CXX} gcc)
message($ENV{CXX})
unset(ENV{CXX})

# list list(操作 变量 值.... 返回值)
list(APPEND listvalue talps wwt)
list(LENGTH listvalue len)
list(FIND listvalue talps index)
list(REMOVE_ITEM listvalue talps)
message(${len})
message(${index})
message(${listvalue})

# list(插入 变量 位置 要插入的地方)
list(INSERT listvalue 1 a4)
message(${listvalue})
list(SORT listvalue)
message(${listvalue})
list(REVERSE listvalue)
message(${listvalue})
