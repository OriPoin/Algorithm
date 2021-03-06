# B+ tree

要求参见[BPlusTree.md](BPlusTree.md)

- [x] 插入
- [x] 查询
- [x] 修改
- [X] 删除
- [X] 序列化
- [X] 反序列化
- [X] 全内存模式
- [X] 文件系统模式

## 编译

```bash
make
```

## 测试

### 生成数据

正序 100 个数字

```bash
./build/gendb -o 100 # ${DB_SIZE}
```

倒序 100 个数字

```bash
./build/gendb -r 100 # ${DB_SIZE}
```

乱序 20 个数字

```bash
./build/gendb -R 100 # ${DB_SIZE}
```

### 测试

需要使用 gdb 查看树的建立情况

加载数据

进入自建命令行

```bash
bpt> init ${DBNAME} #test.db | ${DB_SIZE}InOrder.db | ${DB_SIZE}Random.db
```

查询数据

进入自建命令行

```bash
bpt> select ${KEY}
```

内建命令行不支持环境变量，以上作为演示，例如`bpt> init 100Random.db`和`bpt> select 10`。

输出如下：

```bash
bpt> select 088
Index: 088
Value: 088
```

修改数据，然后查询

```bash
bpt> update ${KEY} ${NEW_VALUE}
```

完整测试如下：

```bash
bpt> init 100Random.db
bpt> update 050 500
Index: 050
Value: 050 => 500
bpt> select 050
Index: 050
Value: 500
bpt> update 123 123
No such key
```

删除

```bash
bpt> delete ${KEY}
```

完整测试如下：

```bash
bpt> init 30Random.db
bpt> delete 19
Deleted record
Index: 19
Value: 19
bpt> delete 050
No such key
```

序列化与反序列化

```bash
bpt> save
bpt> save ${dbname}
```

```bash
bpt> load ${dbname}
```

操作模式

```bash
bpt> setmode fs		#模拟文件系统B+树，关键字存放在内存，数据存放在文件
bpt> setmode mem	#全内存B+树，全部存放在内存
```
