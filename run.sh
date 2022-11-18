#!/usr/bin/env bash

echo '正在启动MongoDB'
service mongodb start

echo '正在后台启动网站服务'
echo -n '如果jar包与算法库不在同一文件夹下, 请输入jar包所在目录的完整路径与算法库所在的完整路径, 否则直接回车即可\n'
read -p 'Jar包路径: ' jar_path
read -p '算法库路径: ' algorithm
if [ -z $jar_path ];then
    jar_path="."
fi
if [ -z $algorithm ]; then
    algorithm=$(cd "$(dirname "$0")";pwd)
fi
# echo ${jar_path}
# echo ${algorithm}
nohup java -jar ${jar_path}/backup-system-web-0.0.1-SNAPSHOT.jar > web.log --algorithm.home="${algorithm}/" &

local_ip=$(curl http://pv.sohu.com/cityjson 2>> /dev/null | awk -F '"' '{print $4}')
echo '系统启动完成, 请打开'${local_ip}':9090'