#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

using namespace std;

// 递归列出所有目录及文件
void recursion_scan_dir_file(const char* dir, int depth) {
    DIR* p_dir = NULL;
    struct dirent* p_entry = NULL;
    struct stat statbuf;

    if ((p_dir = opendir(dir)) == NULL) {
        printf("can't open dir.\n");
        return;
    }

    chdir(dir);

    while (NULL != (p_entry = readdir(p_dir))) {  // 获取下一级目录信息

        lstat(p_entry->d_name, &statbuf);  // 获取下一级成员属性

        if (S_IFDIR & statbuf.st_mode) {  // 判断下一级成员是否是目录
            if (strcmp(".", p_entry->d_name) == 0 || strcmp("..", p_entry->d_name) == 0) continue;

            printf("%*s%s/\n", depth, "", p_entry->d_name);
            recursion_scan_dir_file(p_entry->d_name, depth + 4);  // 扫描下一级目录的内容
        } else {
            printf("%*s%s\n", depth, "", p_entry->d_name);  // 输出属性不是目录的成员
        }
    }
    chdir("..");  // 回到上级目录
    closedir(p_dir);
}

//列出一个目录下所有文件
void scan_one_dir(const char* dir_name) {
    if (NULL == dir_name) {
        cout << " dir_name is null ! " << endl;
        return;
    }

    struct stat s;
    lstat(dir_name, &s);
    if (!S_ISDIR(s.st_mode)) {
        return;
    }

    struct dirent* filename;
    DIR* dir;
    dir = opendir(dir_name);
    if (NULL == dir) {
        return;
    }

    int iName = 0;
    while ((filename = readdir(dir)) != NULL) {
        if (strcmp(filename->d_name, ".") == 0 || strcmp(filename->d_name, "..") == 0) continue;

        char wholePath[128] = {0};
        sprintf(wholePath, "%s/%s", dir_name, filename->d_name);

        cout << "wholePath= " << wholePath << endl;
    }
}

main(int argc, char* argv[]) {
    const char* path = "/data/AudioDetection/hit/AudioAnalysisEngine";
    // scan_one_dir(path);
    recursion_scan_dir_file(path, 1);  // depth 是指空格数，不是遍历深度
    return 0;
}