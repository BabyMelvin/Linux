# !/bin/bash

# 1.读取列表变量
list=" Alabama Alaska Arizona"
list=$list" Conecticut"

# 作为一个完成的输出
for state in $list
do
    # 这里只输出一条语句
    echo "have you ever visited $state"
done

# 2.列表为复杂值
for test in I don\'t know if "this'll" work
do 
    # 作为列表输出,多条输出
    echo "word:$test"
done
