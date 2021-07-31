# B+ tree

要求参见`./BPlusTree.md`

- [X] 插入
- [ ] 查询
- [ ] 修改
- [ ] 删除

## 编译

```bash
make
```

## 测试

### 生成数据

正序100个数字

```bash
./build/gendb -o 100 # ${DB_SIZE}
```

倒序100个数字

```bash
./build/gendb -r 100 # ${DB_SIZE}
```

乱序20个数字

```bash
./build/gendb -R 100 # ${DB_SIZE}
```

### 测试

需要使用gdb查看树的建立情况

加载数据

```bash
./build/bpt -i ${DBNAME} #test.db | ${DB_SIZE}InOrder.db | ${DB_SIZE}Random.db
```
