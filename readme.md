### 开发环境

* 操作系统

  * Distributor ID: Ubuntu
  * Description:    Ubuntu 20.04 LTS
  * Release:        20.04
  * Codename:       focal

### 目前已实现功能
* 基础功能：备份、恢复、检查合法性
* 打包解包
* 元数据支持

### 代码文件

* `basefunction.h` 基础功能，包括打包和元数据
* `AES.h`  AES对称加密算法实现
* `encrypt and decrypt.h`  文件加密接口实现
* `inotify.h`  实时感知接口实现

### 版本控制（git）

* 指令集
  * git push  （建立项目并上传）
  * git pull origin master 下拉

* start  建立项目
  * `start-0x`  建立项目，完成基础功能
  * `start-1.x` 基础功能实现
  * `start-2.x` 额外功能
    * `start-2.1` 实现加密解密，加密文件后缀为`.cpt`的二进制文件

### 程序使用
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`
* `cd ..`
* `./build/code/backup [path] [0 backup/1 recover/2 check] [key]   (注意必须使用绝对路径)`

### To Do
* 加密


### docker

* `service docker start ` 启用docker
