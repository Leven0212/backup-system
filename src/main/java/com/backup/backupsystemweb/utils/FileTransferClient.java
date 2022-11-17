package com.backup.backupsystemweb.utils;

/**
 * @ClassName FileTransferClient
 * @Description TODO
 * @Author leven
 * @Date 2022/11/16
 */

import java.io.*;
import java.net.Socket;
import java.text.DecimalFormat;

public class FileTransferClient extends Socket {
    private String pathname;
    
    private static final String SERVER_IP = "82.157.242.214"; // 服务端IP
    private static final int SERVER_PORT = 8899; // 服务端端口

    private Socket client;

    private static DecimalFormat df = null;

    private FileInputStream fis;

    private DataOutputStream dos;

    private DataInputStream dis;

    private FileOutputStream fos;

    /**
     * 构造函数<br/>
     * 与服务器建立连接
     * @throws Exception
     */
    public FileTransferClient(String _pathname) throws Exception {
        super(SERVER_IP, SERVER_PORT);
        this.client = this;
        this.pathname = _pathname;
        System.out.println("Cliect[port:" + client.getLocalPort() + "] 成功连接服务端");
    }

    /**
     * 向服务端传输文件
     * @throws Exception
     */
    public void sendFile(String name) throws Exception {
        try {
            File file = new File(pathname+"backup/" + name + "/"+name);
            File file_huf = new File(pathname+"backup/" + name + "/" + name + ".huf");
            File file_cpt = new File(pathname+"backup/" + name + "/" + name + ".huf.cpt");
            if(file.exists() || file_huf.exists() || file_cpt.exists()) {
                if(file_huf.exists()) file = file_huf;
                else if(file_cpt.exists()) file = file_cpt;
                fis = new FileInputStream(file);
                dos = new DataOutputStream(client.getOutputStream());

                // 上传文件标记、文件名和长度
                dos.writeUTF("send");
                dos.flush();
                dos.writeUTF(file.getName());
                dos.flush();
                dos.writeLong(file.length());
                dos.flush();

                // 开始传输文件
                System.out.println("======== 开始传输文件 ========");
                byte[] bytes = new byte[1024];
                int length = 0;
                long progress = 0;
                while((length = fis.read(bytes, 0, bytes.length)) != -1) {
                    dos.write(bytes, 0, length);
                    dos.flush();
                    progress += length;
                    System.out.print("| " + (100*progress/file.length()) + "% |");
                }
                System.out.println();
                System.out.println("======== 文件传输成功 ========");
                file.delete();
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if(fis != null)
                fis.close();
            if(dos != null)
                dos.close();
            client.close();
        }
    }

    /**
     * 从服务端获取文件
     * @throws Exception
     */
    public void getFile(String name) throws Exception {
        try {
            File file = new File(name);
//            fis = new FileInputStream(file);
            dos = new DataOutputStream(client.getOutputStream());

            // 获取文件标记、文件名和长度
            dos.writeUTF("get");
            dos.flush();
            dos.writeUTF(file.getName());
            dos.flush();

            // 开始传输文件
            System.out.println("======== 开始获取文件 ========");
            dis = new DataInputStream(client.getInputStream());
            String fileName = dis.readUTF();
//            System.out.println(fileName);
            if(!fileName.equals("文件不存在")){
                long fileLength = dis.readLong();
                File directory = new File(pathname+"backup");
                String tmp = fileName;
                int i = 0;
                while(i < fileName.length() && fileName.charAt(i) >= '0' && fileName.charAt(i) <= '9') i++;
                tmp = fileName.substring(0, i);
                File directory_inner = new File(pathname+"backup/" + tmp);
                if(!directory.exists()) {
                    directory.mkdir();
                }
                if(!directory_inner.exists()) {
                    directory_inner.mkdir();
                }
                File files = new File(directory_inner.getAbsolutePath() + File.separatorChar + fileName);
                fos = new FileOutputStream(files);

                // 开始接收文件
                byte[] bytes = new byte[1024];
                int length = 0;
                while((length = dis.read(bytes, 0, bytes.length)) != -1) {
                    fos.write(bytes, 0, length);
                    fos.flush();
                }
                System.out.println("======== 文件接收成功 [File Name：" + fileName + "] [Size：" + getFormatFileSize(fileLength) + "] ========");
//                System.out.println("======== 文件接收成功 ========");
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if(fis != null)
                fis.close();
            if(dos != null)
                dos.close();
            client.close();
        }
    }

    /**
     * 格式化文件大小
     * @param length    文件长度
     * @return          格式化结果
     */
    private String getFormatFileSize(long length) {
        double size = ((double) length) / (1 << 30);
        if(size >= 1) {
            return df.format(size) + "GB";
        }
        size = ((double) length) / (1 << 20);
        if(size >= 1) {
            return df.format(size) + "MB";
        }
        size = ((double) length) / (1 << 10);
        if(size >= 1) {
            return df.format(size) + "KB";
        }
        return length + "B";
    }

    /**
     * 哈希文件路径
     * @param path  文件路径
     * @return      哈希结果
     */
    public int hash(String path) {
        long tot = 0;
        for(int i=0;i < path.length(); i++) {
            tot = (path.charAt(i) * 7 + tot) % 1000000007;
        }
        return (int)tot;
    }

    /**
     * 入口
     * @param args
     */
    public static void main(String[] args) {
        try {
            FileTransferClient client = new FileTransferClient("/home/ubuntu/backup-system/"); // 启动客户端连接
//            client.sendFile("/Users/leven/Desktop/test/src/main/java/org/example/16891"); // 传输文件
            client.getFile("16891");
            client.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

}