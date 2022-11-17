package org.example;

/**
 * @ClassName FileTransferServer
 * @Description TODO
 * @Author leven
 * @Date 2022/11/16
 */
import java.net.ServerSocket;
import java.net.Socket;

public class FileTransferServer extends ServerSocket {

    private static final int SERVER_PORT = 8899; // 服务端端口

    public FileTransferServer() throws Exception {
        super(SERVER_PORT);
    }

    /**
     * 使用线程处理每个客户端传输的文件
     * @throws Exception
     */
    public void load() throws Exception {
        while (true) {
            // server尝试接收其他Socket的连接请求，server的accept方法是阻塞式的
            Socket socket = this.accept();
            // System.out.println("=====服务器连接成功=====");
            /**
             * 我们的服务端处理客户端的连接请求是同步进行的， 每次接收到来自客户端的连接请求后，
             * 都要先跟当前的客户端通信完之后才能再处理下一个连接请求。 这在并发比较多的情况下会严重影响程序的性能，
             * 为此，我们可以把它改为如下这种异步处理与客户端通信的方式
             */
            // 每接收到一个Socket就建立一个新的线程来处理它
            new Thread(new Task(socket)).start();
        }
    }
    
    /**
     * 入口
     * @param args
     */
    public static void main(String[] args) {
        try {
            FileTransferServer server = new FileTransferServer(); // 启动服务端
            System.out.println("=====服务器启动成功，等待连接=====");
            server.load();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}