package com.backup.backupsystemweb.utils;


import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.io.FileInputStream;

/**
 * @ClassName Algorithm
 * @Description TODO
 * @Author leven
 * @Date 2022/11/5
 */

@Service
public class Algorithm {
    @Value("${algorithm.home}")
    private String pathname;

    public List<String> deal(List<String> str) {
        String key = str.get(0);
        if(str.get(0).equals("backup")) {
            str.remove(0);
            if(str.get(str.size()-1) == null) {
                str.remove(str.size()-1);
            }
//            添加备份参数
            str.add(1, "0");
            if(str.get(2).equals("jichu")) {
                str.set(2, "0");
            } else if (str.get(2).equals("yasuo")) {
                str.set(2, "1");
            } else {
                str.set(2, "2");
            }
        } else if (str.get(0).equals("recover")) {
            str.remove(0);
            if(str.get(str.size()-1) == null) {
                str.remove(str.size()-1);
            }
//            添加恢复参数
            str.add(1, "1");
            if(str.get(2).equals("jichu2")) {
                str.set(2, "0");
            } else if (str.get(2).equals("jieya")) {
                str.set(2, "1");
            } else {
                str.set(2, "2");
            }
        } else {
            str.remove(0);
//            str.remove(str.size()-1);
            if(str.get(str.size()-1) == null) {
                str.remove(str.size()-1);
            }
//            添加校验参数
            str.add(1, "2");
            if(str.get(2).equals("jichu2")) {
                str.set(2, "0");
            } else if (str.get(2).equals("jieya")) {
                str.set(2, "1");
            } else {
                str.set(2, "2");
            }
//            str.set(2, "0");
        }
        String name = str.get(0);
//        if(name.substring(name.length()-1).equals("/")) str.set(0, name.substring(0, name.length()-1));
        String ans = connect(str);
        List<String> strList = new ArrayList<>();
        strList.add(ans);
        if(key.equals("check") && ans.equals("校验后发现备份存在问题")) {
            List<String> tmp = ReadTxt();
            strList.addAll(tmp);
        }
        return strList;
    }

    private String connect(List<String> str) {
        try {
            Process p = null;
            String line = null;
            ProcessBuilder pb = null;
            BufferedReader stdout = null;

            str.add(0, "./build/code/backup");
            System.out.println(str);
            pb = new ProcessBuilder(str);
//            System.out.println(pathname);
            pb.directory(new File(pathname));
            p = pb.start();
            stdout = new BufferedReader(new InputStreamReader(p.getInputStream()));
            while ((line = stdout.readLine()) != null) {
                System.out.println(line);
            }
            int ret = p.waitFor();
            // System.out.println("return code is " + ret);
            stdout.close();
            String ans = analyze(ret);
            return ans;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * 读取fail.txt并返回其中的错误原因
     * @return fail.txt内容
     */
    public List<String> ReadTxt() {
        List<String> content = new ArrayList<>();
        content.add("存在问题的文件如下：");
        try {
            String encoding="utf8";
//            System.out.println("pathname: " + pathname);
            File file=new File(pathname + "fail.txt");
            if(file.isFile() && file.exists()){ //判断文件是否存在
//                System.out.println(pathname + "fail.txt");
                InputStreamReader read = new InputStreamReader(new FileInputStream(file),encoding);//考虑到编码格式
                BufferedReader bufferedReader = new BufferedReader(read);
                String lineTxt = null;
                while((lineTxt = bufferedReader.readLine()) != null) {
                    content.add(lineTxt);
                }
                read.close();
            }else{
                content.add("找不到指定的文件");
            }
        } catch (Exception e) {
            System.out.println("读取文件内容出错");
            e.printStackTrace();
        }
        return content;
    }

    private String analyze(int code) {
        if(code == 0)
            return "执行成功";
        else if(code == 3) {
            return "文件夹创建失败";
        } else if(code == 4) {
            return "普通文件创建失败";
        } else if(code == 5) {
            return "备份失败";
        } else if(code == 6) {
            return "恢复失败";
        } else if(code == 7) {
            return "校验后发现备份存在问题";
        } else if(code == 8) {
            return "打开文件失败";
        } else if(code == 9) {
            return "打开文件夹失败";
        } else if(code == 10) {
            return "文件名称出错";
        } else if(code == 11) {
            return "删除文件失败，无法进行恢复";
        } else if(code == 12) {
            return "无法修改文件权限，执行失败";
        } else if(code == 13) {
            return "读取（软/硬）链接文件出错";
        } else if(code == 14) {
            return "创建软链接文件出错";
        } else if(code == 15) {
            return "创建硬链接文件出错";
        } else {
            return "由于未知原因导致执行出错";
        }
    }
}
