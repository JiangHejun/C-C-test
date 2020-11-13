/*
 * @Description:
 * @Author: Hejun Jiang
 * @Date: 2020-11-13 10:56:22
 * @LastEditTime: 2020-11-13 11:32:59
 * @LastEditors: Hejun Jiang
 * @Version: v0.0.1
 * @Contact: jianghejun@hccl.ioa.ac.cn
 * @Corporation: hccl
 */
#include <string>
#include <unordered_set>
int main() {
    std::unordered_set<std::string> set;
    set.insert("hccl");
    set.insert("thinkit");
    set.insert("thinkit");
    set.insert("hccl");
    printf("%lu\n", set.size());
    set.erase("jhj");
    set.erase("hccl");
    set.erase("thinkit");
    printf("%lu\n", set.size());
    return 0;
}