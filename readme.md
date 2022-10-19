### 开发环境

* 操作系统

  > Distributor ID: Ubuntu
  > Description:    Ubuntu 20.04 LTS
  > Release:        20.04
  > Codename:       focal

### 代码文件

* AES.h  AES对称加密算法实现
* encrypt and decrypt.h  文件加密接口实现

inotify.h  实时感知接口实现

### 版本控制（git）

* 指令集
  * git push -u origin master （建立项目并上传）
  * git pull origin master 下拉

* start  建立项目

### 使用
* mkdir build
* cd build
* cmake ..
* make

* cd ..
* `./build/code/backup [path] [0 backup/1 recover/2 check] [key]   (注意必须使用绝对路径)`

### To DO
* 备份检查  check
* 加密


### docker

* `service docker start ` 启用docker
