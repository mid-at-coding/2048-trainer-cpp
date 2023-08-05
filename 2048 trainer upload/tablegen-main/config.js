var fs=require("fs");
var config={};
var addkey=function(entry){
  entry=entry.split("=");
  var key=entry[0];
  var value=entry.slice(1).join("=").split("\r").join("");
  config[key]=value;
};
fs.readFileSync("./config.txt","utf-8").split("\n").map(x=>addkey(x));
//console.log(config);
config.spaces*=1;
config.s0*=1;
config.s1*=1;
exports.config=config;