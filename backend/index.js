const express = require('express');
const app = express();
const cors = require('cors');
app.use(cors());
const fs = require('fs');
const {exec}  = require('child_process');

app.get('/', (req, res) => res.send('Hello World!'));

app.get('/get_ans', (req, res) => {
	let para = [], loc = "";
	if (req.query.location == "Beijing") {
		loc = "beijing.dat";
	} else if (req.query.location == "New York City") {
		loc = "nyc.dat";
	} else if (req.query.location == "Gowalla") {
		loc = "gowalla.dat";
	} else if (req.query.location == "Road") {
		loc = "road.dat";
	}
	para.push(loc);
	para.push(req.query.xmin);
	para.push(req.query.ymin);
	para.push(req.query.xmax);
	para.push(req.query.ymax);
	para.push(req.query.epsilon);
	let cmd = `'./privtree/a.out' ${para[0]} ${para[1]} ${para[2]} ${para[3]} ${para[4]} ${para[5]}`;
	console.log(cmd);
	let ls = exec(cmd);
	ls.stderr.on('data', (data) => {
		console.log('stderr ' + data);
	});
	ls.on("close", (code) => {
		console.log("Child Process closed with code " + code);
		encoding = 'utf-8';
		let file = fs.readFileSync('./return.json', encoding);
		res.end(file);
	});
});

app.listen(8888, () => console.log('Example app listening on port 8888!'));