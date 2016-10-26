# -*- cooding: utf-8 -*-
import BaseHTTPServer
class RequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    """deal with request handler and page"""
    #Page = open('webroot/index.html')
    #Page =  '''\
    #    <html>
    #    <body>
    #    <p>Hello, web!</p>
    #    </body>
    #    </html>
    #'''
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/html')
        self.send_header('Content-Length', str(len(self.Page)))
        self.end_headers()
        self.wfile.write(self.Page)

if __name__ == '__main__':
    serverAddr = ('', 8083)
    server = BaseHTTPServer.HTTPServer(serverAddr, RequestHandler)
    server.serve_forever()
