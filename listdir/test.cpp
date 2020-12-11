#include <dirent.h>  //opendir readdir closedir
#include <stdio.h>   //printf
#include <string.h>
#include <sys/stat.h>  // stat
#include <time.h>
#include <unistd.h>  // chdir

#include <iostream>
#include <string>
#include <vector>

int ListDir(const char* dir, std::vector<std::string>& listdir) {
    int n = 0;
    if (!dir) return n;

    DIR* dirfp = opendir(dir);
    if (!dirfp) return n;

    // char buf[80];
    // getcwd(buf, sizeof(buf));
    // printf("current working directory: %s\n", buf);

    // chdir(dir);
    struct stat s;
    struct dirent* filename;
    while ((filename = readdir(dirfp))) {
        if (std::string(filename->d_name) == "." || std::string(filename->d_name) == "..") continue;
        std::string path(std::string(dir) + "/" + std::string(filename->d_name));
        printf("%s\n", path.c_str());
        lstat(path.c_str(), &s);  // 获取下一级成员属性,链接的返回链接属性，stat 返回的时链接真是文件的属性
        printf("%x\n", s.st_mode);
        if (S_ISREG(s.st_mode)) listdir.emplace_back(path);
        n++;
    }
    // chdir(buf);  // 回到上级目录
    // printf("return working directory: %s\n", buf);
    closedir(dirfp);
    return n;
}

int GetOneDirFiles(const char* dir_name, std::vector<std::string>& files) {
    if (!dir_name) return -1;

    auto dir = opendir(dir_name);
    if (!dir) return -2;

    struct dirent* filename;
    while ((filename = readdir(dir)) != NULL) {
        if (std::string(filename->d_name) == "." || std::string(filename->d_name) == "..") continue;
        files.push_back(std::string(dir_name) + "/" + std::string(filename->d_name));
    }
    return closedir(dir);
}

std::string GetCwd(const char* path = NULL) {
    char dir[4096];
    if (path == NULL) {
        if (getcwd(dir, sizeof(dir)) == NULL) {
            perror("GetCwd getcwd");
            return "./";
        }
    } else {
        if (realpath(path, dir) == NULL) {
            perror("GetCwd realpath");
            return path;
        }
    }
    return dir;
}

// "*" is all files, "" is no extsion files
int GetDirFiles(const char* dir_name, std::vector<std::string>& files, int depth = 0, std::string extension = "*", bool abspath = true) {
    if (depth < 0) return -1;
    if (!dir_name) return -2;

    auto p_dir = opendir(dir_name);
    if (!p_dir) return -3;

    struct dirent* p_entry;
    struct stat statbuf;
    while ((p_entry = readdir(p_dir))) {
        if (std::string(p_entry->d_name) == "." || std::string(p_entry->d_name) == "..") continue;
        std::string sym = (*(--std::string(dir_name).end())) == '/' ? "" : "/";
        std::string path(std::string(dir_name) + sym + std::string(p_entry->d_name));
        lstat(path.c_str(), &statbuf);                 // 获取下一级成员属性,链接的返回链接属性，stat 返回的时链接真是文件的属性
        if (S_ISDIR(statbuf.st_mode) && depth != 1) {  // 判断下一级成员是否是目录, ==1禁止
            int rlt = GetDirFiles(path.c_str(), files, depth == 0 ? 0 : depth - 1, extension);
            if (rlt < 0) return rlt;
        }  // 扫描下一级目录的内容
        else if (S_ISREG(statbuf.st_mode)) {
            if (extension != "*") {
                const char* ret = strrchr(p_entry->d_name, '.');
                if (ret) {  // have ext
                    if (std::string(ret) != extension) continue;
                } else if (extension != "")  // have noext and extension not is ""
                    continue;

                if (ret && std::string(ret) != extension) continue;
            }
            files.emplace_back(abspath ? GetCwd(path.c_str()) : path);
        }
    }
    closedir(p_dir);
    return 0;
}

int main() {
    int rec;
    const char* dir = "./tmp";
    // const char* dir = "/data/docs/";
    // const char* dir = "./vscode/";
    std::vector<std::string> filesV;

    // filesV.clear();
    // rec = GetOneDirFiles(dir, filesV);
    // if (rec < 0) {
    //     fprintf(stderr, "GetOneDirFiles error, %d\n", rec);
    //     return -1;
    // } else {
    //     for (auto file : filesV) printf("%s\n", file.c_str());
    // }

    filesV.clear();
    rec = GetDirFiles(dir, filesV, 3);
    // rec = GetDirFiles(dir, filesV, -1, "*", false);
    if (rec < 0) {
        fprintf(stderr, "GetDirFiles error, %d\n", rec);
        return -1;
    } else {
        for (auto file : filesV) printf("%s\n", file.c_str());
    }

    // filesV.clear();
    // rec = ListDir(dir, filesV);
    // if (rec < 0) {
    //     fprintf(stderr, "ListDir error, %d\n", rec);
    //     return -1;
    // } else {
    //     for (auto file : filesV) printf("%s\n", file.c_str());
    // }

    // std::string name("jhwq");
    // // auto idx = name.substr(10, 1); //x
    // auto s = name.substr(2, 10);  // x
    // printf("%s\n", s.c_str());

    // printf("%lld\n", time(NULL));
    return 0;
}
