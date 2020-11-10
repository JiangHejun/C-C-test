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

// �ݹ��г�����Ŀ¼���ļ�
void recursion_scan_dir_file(const char* dir, int depth) {
    DIR* p_dir = NULL;
    struct dirent* p_entry = NULL;
    struct stat statbuf;

    if ((p_dir = opendir(dir)) == NULL) {
        printf("can't open dir.\n");
        return;
    }

    chdir(dir);

    while (NULL != (p_entry = readdir(p_dir))) {  // ��ȡ��һ��Ŀ¼��Ϣ

        lstat(p_entry->d_name, &statbuf);  // ��ȡ��һ����Ա����

        if (S_IFDIR & statbuf.st_mode) {  // �ж���һ����Ա�Ƿ���Ŀ¼
            if (strcmp(".", p_entry->d_name) == 0 || strcmp("..", p_entry->d_name) == 0) continue;

            printf("%*s%s/\n", depth, "", p_entry->d_name);
            recursion_scan_dir_file(p_entry->d_name, depth + 4);  // ɨ����һ��Ŀ¼������
        } else {
            printf("%*s%s\n", depth, "", p_entry->d_name);  // ������Բ���Ŀ¼�ĳ�Ա
        }
    }
    chdir("..");  // �ص��ϼ�Ŀ¼
    closedir(p_dir);
}

//�г�һ��Ŀ¼�������ļ�
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
    recursion_scan_dir_file(path, 1);  // depth ��ָ�ո��������Ǳ������
    return 0;
}