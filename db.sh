#!/bin/bash
# key，value方式存储到database文件中
insert() {
	echo "$1,$2" >> database
}
db_get() {
# 查找到全部索引，并且将key去掉，获取重复记录的最后一个
	grep "^$1," database | sed -e "s/^$1,//" | tail -n 1
}

insert 123456 '{"name":"London","attractions":["Big Ben","London Eye"]}'
db_get 123456
