## 算法端

### 开发环境

* 操作系统

  * Distributor ID: Ubuntu
  * Description:    Ubuntu 20.04 LTS
  * Release:        20.04
  * Codename:       focal

### 目前已实现功能   107
* 基础功能：备份、恢复、检查合法性   40
* 打包解包 10
* 元数据支持 10
* 特殊文件支持 10
* 压缩解压 10 
* 加密解密 7
* Web界面 10
* 网盘备份 10


### 版本控制（git）

* 指令集
  * git push  （建立项目并上传）
  * git pull origin master 下拉

* start  建立项目
  * `start-0x`  建立项目，完成基础功能
  * `start-1.x` 基础功能实现
  * `start-2.x` 
    * `start-2.1` 实现加密解密，加密文件后缀为`.cpt`的二进制文件
    * `start-2.2` 修正加密密码有关问题
    * `start-2.3` 修正backup目录的创建问题
    * `start-2.4` 支持管道、软链接
    * `start-2.5` 支持硬链接
    * `start-2.6` 支持压缩
    * `start-2.7` 定位加密错误，除去加密；添加可选参数
    * `start-2.8` 软链接复制优化；错误处理
    * `start-2.9` 使用openssl进行加密
    * `start-2.10` 生成支持前端的动态链接库（so）
    * `start-2.11` 解决文件夹属性更改错误；调整错误返回
    * `start-2.12` 定位备份错误文件，将错误文件记录在txt中
    * `start-2.13` 修改检查功能问题
  * `start-3x`
    * `start-3.0` 前端测试
    * `start-3.5` 恢复功能错误修正（空行）  

### 程序使用
* `mkdir build`
* `cd build`
* `cmake ..`
* `make`
* `cd ..`
* `./build/code/backup [path] [0 / 1 / 2] [0/1/2] [key] `
  * 注意必须使用绝对路径
  * 0 backup ; 1 recover; 2 check
  * 0 基本功能； 1 基本功能 + 压缩 ； 2 基本功能+压缩+加密
  * 密码长度任意，但有效长度为16位




## backup-system-web

### 版本
#### - v1 环境准备
  - v1.0 创建网站项目
  - v1.1 添加动态链接库.so文件，添加相关依赖
  - v1.2 创建调用.so文件的java库
  - v1.3 改用使用新进程调用算法库的方式

#### - v2 网站前端
  - v2.1 导入前端模板
  - v2.2 修改相关配置
  - v2.3 调整前端板式
  - v2.4 完成界面设计
  - v2.5 添加校验功能
  - v2.6 结果自动刷新
  - v2.7 添加错误模版

#### - v3 网站后端
  - v3.1 备份部分基本实现
  - v3.2 恢复和校验部分基本实现
  - v3.3 数据库存储备份方式完成
  - v3.4 实现算法库路径参数化 **(失败)**
  - v3.5 校验出错定位展示
  - v3.6 解决v3.4中的失败问题
  - v3.7 实现文件路径合法化
  - v3.8 实现错误结果的展示
  - v3.9 解决了文件路径合法性校验遗漏问题，解决了校验时需要密码问题，解决了空路径或空密码问题
  - v3.10 解决了目前已知的bug。
  - v3.11 添加环境配置指南
  - v3.12 添加网站启动脚本，增加网盘传输功能



==================================================

### TODO：
  - 调用算法前检查文件路径是否合法，如果最后以`“/”`结尾，则删去`“/”`  **=====解决=====**
  - 实现数据库的插入功能  **=====解决=====**
  - 将错误代码与文字提示绑定  **=====解决=====**
  - 解决特殊情况：用户选择不需要密码形式，但实际备份文件是加密的，此时现有程序没有检查密码，易导致出错。  **=====解决=====**
  - 在写入和读取数据库前检查路径合法性，即最后有无`“/”`结尾，并进行处理  **=====解决=====**
  - 校验时需要使用密码，需读取数据库来获得  **=====解决=====**
  - 解决当用户输入文件路径为空或密码为空时报错问题  **=====解决=====**

### 执行方法：
#### 一、生成 `jar` 包
手动克隆代码库并将网站源码打成 `jar` 包，或直接下载已打包好的 `jar` 包
##### 下载代码并打包
```shell
# 下载代码库并进行打包
> git clone https://github.com/Leven0212/backup-system-web.git
> mv backup-system-web-master backup-system-web
> cd backup-system-web
> mvn clean package
```
待出现 `build success` 即可，如下图所示：
![img.png](https://raw.githubusercontent.com/Leven0212/picture-club/main/202211081754823.png)

##### 直接下载已打包好的 `jar` 包
```shell
> wget https://raw.githubusercontent.com/Leven0212/backup-system/master/backup-system-web-0.0.1-SNAPSHOT.jar
```
#### 二、配置Java环境和MongoDB环境
`Java`版本 >= 1.8 \
`MongoDB`版本 >= 3.6

```shell
# 下载 Java 环境
> sudo apt install openjdk-8-jdk
# 下载 MongoDB 环境
> sudo apt install mongodb
```
* 如果要手动执行代码打包，还需配置`maven`环境
```shell
> sudo apt install maven
```
#### 三、启动MongoDB和web服务
```shell
# 启动MongoDB
> service mongodb start
# 启动web服务，其中参数 xxx 处填写算法库根目录
> java -jar backup-system-web-0.0.1-SNAPSHOT --algorithm.home="xxx"
# 启动网盘服务端
> /usr/bin/env /usr/lib/jvm/java-8-openjdk-amd64/jre/bin/java -cp /home/ubuntu/test/target/classes org.example.FileTransferServer
```