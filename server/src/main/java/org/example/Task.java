package org.example;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.math.RoundingMode;
import java.net.Socket;
import java.text.DecimalFormat;

/**
 * 处理客户端传输过来的文件线程类
 */
class Task implements Runnable {

    private Socket socket;

    private DataInputStream dis;

    private FileOutputStream fos;

    private FileInputStream fis;

    private DataOutputStream dos;

    public Task(Socket socket) {
        this.socket = socket;
    }

    private static DecimalFormat df = null;

    static {
        // 设置数字格式，保留一位有效小数
        df = new DecimalFormat("#0.0");
        df.setRoundingMode(RoundingMode.HALF_UP);
        df.setMinimumFractionDigits(1);
        df.setMaximumFractionDigits(1);
    }

    @Override
    public void run() {
        try {
            System.out.println("connet from ==> " + socket.getInetAddress().getHostName());
            dis = new DataInputStream(socket.getInputStream());

            // 文件名和长度
            String func = dis.readUTF();
            // System.out.println(func);
            if (func.equals("send")) {
                String fileName = dis.readUTF();
                long fileLength = dis.readLong();
                File directory = new File("./backup_server");
                if (!directory.exists()) {
                    directory.mkdir();
                }
                File file = new File(directory.getAbsolutePath() + File.separatorChar + fileName);
                // System.out.println(file.getName());
		        fos = new FileOutputStream(file);

                // 开始接收文件
                byte[] bytes = new byte[1024];
                int length = 0;
                while ((length = dis.read(bytes, 0, bytes.length)) != -1) {
                    fos.write(bytes, 0, length);
                    fos.flush();
                }
                System.out.println("======== 文件接收成功 [File Name：" + fileName + "] [Size：" + getFormatFileSize(fileLength)
                        + "] ========");
            } else {
                String fileName = dis.readUTF();
                File file = new File("./backup_server/" + fileName);
                File file_huf = new File("./backup_server/" + fileName + ".huf");
                File file_cpt = new File("./backup_server/" + fileName + ".huf.cpt");
                if(file.exists() || file_huf.exists() || file_cpt.exists()) {
                    if(file_huf.exists()) file = file_huf;
                    else if(file_cpt.exists()) file = file_cpt;
                    fis = new FileInputStream(file);
                    dos = new DataOutputStream(socket.getOutputStream());

                    // 文件名和长度
                    dos.writeUTF(file.getName());
                    dos.flush();
                    dos.writeLong(file.length());
                    dos.flush();

                    System.out.println("======== 开始传输文件 ========");
                    byte[] bytes = new byte[1024];
                    int length = 0;
                    long progress = 0;
                    while ((length = fis.read(bytes, 0, bytes.length)) != -1) {
                        dos.write(bytes, 0, length);
                        dos.flush();
                        progress += length;
                        System.out.print("| " + (100 * progress / file.length()) + "% |");
                    }
                    System.out.println();
                    System.out.println("======== 文件传输成功 ========");
                    file.delete();
                } else {
                    dos = new DataOutputStream(socket.getOutputStream());
                    dos.writeUTF("文件不存在");
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                if (fos != null)
                    fos.close();
                if (dis != null)
                    dis.close();
                socket.close();
            } catch (Exception e) {
            }
        }
    }

    /**
     * 格式化文件大小
     * @param length
     * @return
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
}
