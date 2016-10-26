import java.io.InputStream;
import java.io.IOException;

public class Request {

    private InputStream input;
    private String uri;

    public Request(InputStream input) {
        this.input = input;
    }

    // from InputStream to get iht information of request
    public void parse() {
        StringBuffer request = new StringBuffer(2048);
        int i;
        byte[] buffer = new byte[2048];
        try {
            i = input.read(buffer);
        } catch (IOException e) {
            e.printStackTrace();
            i = -1;
        }
        for (int j = 0; j < i; j++) {
            request.append((char) buffer[j]);
        }
        System.out.println(request.toString());
        System.out.print(request.toString());
        uri = parseUri(request.toString());
    }

    /**
     *
     * requestString like below:
     * GET /index.html HTTP/1.1
     * Host: localhost:8080
     * Connection: keep-alive
     * Cache-Control: max-age=0
     * ...
     * to get the string of /index.html
     */
    private String parseUri(String requestString) {
        int index1, index2;
        index1 = requestString.indexOf(' ');
        if (index1 != -1) {
            index2 = requestString.indexOf(' ', index1 + 1);
            if (index2 > index1)
                return requestString.substring(index1 + 1, index2);
        }
        return null;
    }

    public String getUri() {
        return uri;
    }

}