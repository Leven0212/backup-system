package com.backup.backupsystemweb.utils;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

/**
 * @ClassName Judge
 * @Description TODO
 * @Author leven
 * @Date 2022/11/2
 */
@Service
public class Judge {
    @Autowired
    DataBase dataBase;

    public String normal(String name) {
        if(name == null) return "文件路径或密码不能为空，请检查后重试！";
        if(name.substring(name.length()-1).equals("/")) name = name.substring(0, name.length()-1);
        return name;
    }

    /**
     * 判断是否需要密码
     * @param file      文件名
     * @param method    备份或加密类型
     * @return          需要密码则返回true，否则返回false；
     */
    public boolean UsePasswd(String file, String method) {
        if(method.equals("jiami") || method.equals("jiemi")) {
            return true;
        }
        return false;
    }

    /**
     * 判断当前待恢复文件是否有被加密备份的文件
     * @param file  文件名
     * @return      有则返回true，否则返回false
     */
    public boolean HavePasswd(String file) {
        FileInfo fileInfo = (FileInfo) dataBase.findOne(file);
        if(fileInfo == null || !fileInfo.isUsepasswd()) return false;
        else return true;
    }

    /**
     * 检查密码是否正确
     * @param file      文件名
     * @param passwd    密码
     * @return          正确返回true，否则返回false
     */
    public boolean CheckPasswd(String file, String passwd) {
        FileInfo fileInfo = (FileInfo) dataBase.findOne(file);
        if(!fileInfo.getPasswd().equals(passwd)) return false;
        else return true;
    }

    /**
     * 判断文件加密信息是否在数据库中
     * @param file      文件名
     * @param method    备份方式
     * @param passwd    加密密码（如果使用加密方式）         
     */
    public void InDatabase(String file, String method, String passwd) {
        FileInfo fileInfo = (FileInfo) dataBase.findOne(file);
        boolean use;
        int level = 0;
        if(!method.equals("jiami")) {
            use = false;
            passwd = null;
            if(method.equals("jichu")) {
                level = 0;
            } else {
                level = 1;
            }
        } else {
            use = true;
            level = 2;
        }
        if(fileInfo == null) {
            /*FileInfo fInfo = (FileInfo) */dataBase.insert(file, use, passwd, level);
            // System.out.println(fInfo.getName() + " " + fInfo.isUsepasswd() + " " + fInfo.getPasswd());
        } else {
            dataBase.update(file, use, passwd, level);
        }
    }

    public String GetPasswd(String file) {
        FileInfo fileInfo = (FileInfo) dataBase.findOne(file);
        return fileInfo.getPasswd();
    }

    public String GetLevel(String file) {
        String ans;
        FileInfo fileInfo = (FileInfo) dataBase.findOne(file);
        int level = fileInfo.getLevel();
//        System.out.println("level: " + level);
        if(level == 0) ans = "jichu2";
        else if(level == 1) ans = "jieya";
        else ans = "jiemi";
        return ans;
    }

    public List<List<String>> GetAll() {
        List<FileInfo> fileInfoList = (List<FileInfo>) dataBase.findAll();
        List<List<String>> result = new ArrayList<>();
        for(int i = 0; i < fileInfoList.size(); i++) {
            FileInfo fileInfo = fileInfoList.get(i);
            List<String> tmp = new ArrayList<>();
            tmp.add(fileInfo.getName());
            int level = fileInfo.getLevel();
            if(level == 0) tmp.add("基础备份");
            else if(level == 1) tmp.add("压缩备份");
            else tmp.add("加密备份");
            result.add(tmp);
        }
        return result;
    }
}
