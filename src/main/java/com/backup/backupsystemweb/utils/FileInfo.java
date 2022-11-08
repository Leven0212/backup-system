package com.backup.backupsystemweb.utils;

import org.springframework.data.mongodb.core.mapping.Document;

import lombok.Data;

/**
 * @ClassName File
 * @Description TODO
 * @Author leven
 * @Date 2022/11/2
 */

@Data
@Document("files")
public class FileInfo {
    private String name;
    private boolean usepasswd;
    private String passwd;
    private int level;  // 用于表示备份等级，0 表示基础，1 表示压缩，2 表示加密
}
