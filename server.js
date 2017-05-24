var fs = require('fs'); 
var http = require('http'); 
var jpeg = require('jpeg-js');
var exec = require('child_process').exec;

http.createServer(function (req, res) {
	res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Access-Control-Request-Method', '*');
	res.setHeader('Access-Control-Allow-Methods', 'OPTIONS, GET');
	res.setHeader('Access-Control-Allow-Headers', '*');
	
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    req.on('data', function (chunk) {
        //console.log('GOT DATA! : ' + chunk);
        var data = JSON.parse(chunk.toString());
        var bitmap = new Buffer(data.img_data, 'base64');
        var imageData = jpeg.decode(bitmap);
        var fname = (new Date()).getTime() + ".raw";
		fs.writeFileSync(fname, imageData.data);
		var cmd = './imgproc border/' + data.img_id + '.txt ' + fname + ' ' + imageData.width + ' ' + imageData.height;
		console.log(cmd);
		exec(cmd, function(error, stdout, stderr) {
			fs.unlinkSync(fname);
			console.log(stdout);
			res.end(stdout);
		});
    });
    //res.end('no data'); 
}).listen(8080);
