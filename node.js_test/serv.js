var http = require('http')
	, path = require('path')
	, fs = require('fs')
	, formidable = require('formidable')

function getContentPath(requestUrl) {
	var filePath = requestUrl.indexOf('/images/') === 0 ? '.' + requestUrl : './content' + requestUrl
	if (filePath == './content/') {
		filePath = './content/index.html'
	}
	return filePath
}

function getContentType(filePath) {
	var extname = path.extname(filePath)
		, contentType
	switch (extname) {
		case '.js':
			contentType = 'text/javascript'
			break
		case '.jpg':
		case '.jpeg':
			contentType = 'image/jpeg'
			break
		case '.gif':
			contentType = 'image/gif'
			break
		case '.png':
			contentType = 'image/png'
			break
		default:
			contentType = 'text/html; charset=utf-8'
			break
	}
	return contentType
}


function processPost(request, response, callback) {
        console.log("postert");
	var form = new formidable.IncomingForm()
		, filename
	form.parse(request, function(error, fields, files){
                console.log("parse");
		if (error) {
                        console.log("errer");
			response.writeHead(500);
			response.end();
		} else {
                        console.log("ok");
			filename = "images/" + files.file.name
			console.log("ok");
			fs.readFile(files.file.path, function(error, data){
				if (error) throw error
				
				fs.writeFile(filename, data, function(error) {
					if (error) throw error
					response.writeHead(201, { 
						'Content-Type': 'application/json', 
						'Location': '/' + filename
					})
					response.end('{"image":"/' + filename + '"}', 'utf-8')
				})
			})
		}
	})
	if(callback) {
		callback(request, response)
	}
}

function processGet(request, response, callback) {
	filePath = getContentPath(request.url)
	fs.exists(filePath, function(exists) {
		if (exists) {
			fs.readFile(filePath, function(error, content) {
				if (error) {
					response.writeHead(500)
					response.end()
				} else {
					response.writeHead(200, { 'Content-Type': getContentType(filePath)})
					response.end(content, 'utf-8')
				}
			})
		} else {
			response.writeHead(404)
			response.end()
		}
	})
	if(callback) {
		callback(request, response)
	}
}

http.createServer(function (request, response) {
	console.log(request.method, request.url)
	
	switch(request.method.toLowerCase()){
		case 'post':
                        console.log("post");
			processPost(request, response)
			break
		
		default:
			processGet(request, response)
			break
	}

}).listen(8080)

console.log('Server running @ 8080');
