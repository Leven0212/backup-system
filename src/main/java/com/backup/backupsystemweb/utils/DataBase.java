package com.backup.backupsystemweb.utils;

import com.mongodb.client.result.UpdateResult;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.core.query.Query;
import org.springframework.data.mongodb.core.query.Update;
import org.springframework.stereotype.Service;

import java.util.List;

/**
 * @ClassName DataBase
 * @ClassName DataBase
 * @Description TODO
 * @Author leven
 * @Date 2022/11/2
 */

@Service
public class DataBase {
    @Autowired
    private MongoTemplate mongoTemplate;

    private static final String COLLECTION_NAME = "files";

    public Object insert(String name, boolean use, String passwd, int level) {
        FileInfo file;
        if(passwd == null) {
            file = new FileInfo();
            file.setName(name);
            file.setUsepasswd(use);
            file.setLevel(level);
        } else {
            // 设置文件信息
            file = new FileInfo();
            file.setName(name);
            file.setUsepasswd(use);
            file.setPasswd(passwd);
            file.setLevel(level);
        }
        // 插入一条文件数据，如果文档信息已经存在就抛出异常
        FileInfo newFileInfo = mongoTemplate.insert(file, COLLECTION_NAME);
        // 输出存储结果
        return newFileInfo;
    }

    public Object findAll() {
        // 执行查询集合中全部文档信息
        List<FileInfo> documentList = mongoTemplate.findAll(FileInfo.class, COLLECTION_NAME);
        // 输出结果
        return documentList;
    }

    public Object findOne(String name) {
        // 设置查询条件参数
        String filename = name;
        // 创建条件对象
        Criteria criteria = Criteria.where("name").is(filename);
        // 创建查询对象，然后将条件对象添加到其中
        Query query = new Query(criteria);
        // 查询一条文档，如果查询结果中有多条文档，那么就取第一条
        FileInfo fileInfo = mongoTemplate.findOne(query, FileInfo.class, COLLECTION_NAME);
        // 输出结果
        return fileInfo;
    }

    public Object update(String name, boolean use, String passwd, int level) {
        String filename = name;
        // 创建条件对象
        Criteria criteria = Criteria.where("name").is(filename);
        // 创建查询对象，然后将条件对象添加到其中
        Query query = new Query(criteria);
        // 创建更新对象,并设置更新的内容
        Update update = new Update().set("usepasswd", use).set("passwd", passwd).set("level", level);
        // 执行更新，如果没有找到匹配查询的文档，则创建并插入一个新文档
        UpdateResult result = mongoTemplate.upsert(query, update, FileInfo.class, COLLECTION_NAME);
        // 输出结果信息
        String resultInfo = "匹配到" + result.getMatchedCount() + "条数据,对第一条数据进行了更改";
        return resultInfo;
    }
}
