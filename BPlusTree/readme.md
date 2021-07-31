# B+ tree

要求参见`./BPlusTree.md`

- [x] 插入
- [x] 查询
- [ ] 修改
- [ ] 删除

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

```bash
./build/bpt -i ${DBNAME} #test.db | ${DB_SIZE}InOrder.db | ${DB_SIZE}Random.db
```

或者进入自建命令行

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
