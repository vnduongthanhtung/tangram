var http = require('http');
var jpeg = require('jpeg-js');
var ffi = require('ffi');
var ref = require('ref');
var ArrayType = require('ref-array');

var FloatArray = ArrayType(ref.types.float);
var int = ref.types.int;

var lib = null;

http.createServer(function (req, res) {

    res.setHeader('Access-Control-Allow-Origin', '*');
	res.setHeader('Access-Control-Request-Method', '*');
	res.setHeader('Access-Control-Allow-Methods', 'OPTIONS, GET');
	res.setHeader('Access-Control-Allow-Headers', '*');
	
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    req.on('data', function (chunk) {
		if(lib == null){
			 lib = ffi.Library('libimg', {
				  'fitOnImage': [int, [FloatArray, FloatArray, FloatArray, int, int, int] ]
				});
		}
        //console.log('GOT DATA! : ' + chunk);
        var data = JSON.parse(chunk.toString());
        var bitmap = new Buffer(data.img_data, 'base64');
        var imageData = jpeg.decode(bitmap);
        var imgR = [];
        var imgG = [];
        var imgB = [];
        for(var i = 0; i < imageData.data.length/4; i++){
			var redValue = imageData.data[4*i];
			var greenValue = imageData.data[4*i+1];
			var blueValue = imageData.data[4*i+2];
			imgR.push(redValue/255.0);
			imgG.push(greenValue/255.0);
			imgB.push(blueValue/255.0);
		}
		
        var ok = lib.fitOnImage(new FloatArray(imgR), new FloatArray(imgG), new FloatArray(imgB), imageData.width, imageData.height, data.img_id);
        console.log("ok=" + ok);
        res.end(ok + '');
    });

}).listen(8080);

/*


https.createServer(options, function (req, res) { 
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
        var imgR = [];
        var imgG = [];
        var imgB = [];
        for(var i = 0; i < imageData.data.length/4; i++){
			var redValue = imageData.data[4*i];
			var greenValue = imageData.data[4*i+1];
			var blueValue = imageData.data[4*i+2];
			imgR.push(redValue/255.0);
			imgG.push(greenValue/255.0);
			imgB.push(blueValue/255.0);
		}
		
        var ok = lib.fitOnImage(new FloatArray(imgR), new FloatArray(imgG), new FloatArray(imgB), imageData.width, imageData.height, data.img_id);
        console.log("ok=" + ok);
        res.end(ok + '');
    });
    //res.end('no data'); 
}).listen(8080);
*/
