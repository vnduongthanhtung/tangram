
var http = require('http');

http.createServer(function (req, res) {

    res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Access-Control-Request-Method', '*');
	res.setHeader('Access-Control-Allow-Methods', 'OPTIONS, GET');
	res.setHeader('Access-Control-Allow-Headers', '*');
	
    console.log('Request received: ');
    console.log('Request recieved: \nmethod: ' + req.method + '\nurl: ' + req.url);

    res.writeHead(200, { 'Content-Type': 'text/plain' });
    req.on('data', function (chunk) {
        console.log('GOT DATA! : ' + chunk);
    });
    res.end('hello');

}).listen(8080);
console.log('Server running on port 8080');
