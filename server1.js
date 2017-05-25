var http = require('http');

http.createServer(function (req, res) {
	res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Access-Control-Request-Method', '*');
	res.setHeader('Access-Control-Allow-Methods', 'OPTIONS, GET');
	res.setHeader('Access-Control-Allow-Headers', '*');
	
    console.log('request received');
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end('Hello');
}).listen(8124);
