import java.net.Socket;
import java.net.ServerSocket;
import java.net.InetAddress;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.io.File;

public class HttpServer {

    public static final String WEB_ROOT = System.getProperty("user.dir") + File.separator + "webroot";

    // The command to close server
    private static final String SHUTDOWN_COMMAND = "/SHUTDOWN";

    public static void main(String[] args) {

        System.out.println(WEB_ROOT.toString());
        HttpServer server = new HttpServer();
        //等待连接请求
        server.await();
    }

    public void await() {
        ServerSocket serverSocket = null;
        int port = 2345;
        try {
            serverSocket = new ServerSocket(port, 1, InetAddress.getByName("10.50.141.78"));
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(1);
        }

        // Waiting for a request
        while (true) {
            Socket socket = null;
            InputStream input = null;
            OutputStream output = null;
            try {
                // Waiting for a connection
                socket = serverSocket.accept();
                input = socket.getInputStream();
                output = socket.getOutputStream();

                // Create Request and parse it
                Request request = new Request(input);
                request.parse();
                // Check wether to shut down the server
                if (request.getUri().equals(SHUTDOWN_COMMAND)) {
                    break;
                }

                Response response = new Response(output);
                response.setRequest(request);
                response.sendStaticResource();

                // close socket
                socket.close();

            } catch (Exception e) {
                e.printStackTrace();
                continue;
            }
        }
    }
}