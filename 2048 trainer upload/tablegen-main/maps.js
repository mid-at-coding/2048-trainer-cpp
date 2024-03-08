// PURPOSE
// as far as I can tell, this file is purely for making two maps: one for hex(but 17. does that have a name?) notation for the tiles,
// and one for the raw numbers, and connecting them together. it then exports these as inverses of each other, ie
// spam[maps[i]] = i.
// spam[i] where i is some alpha not => the numerical representation of that number
// maps[i] where i is some number    => the alpha not.representation of that number

var f=[0,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,1<<17]; // TODO: change to log
var b="0123456789abcdefgh";
var maps={};
var spam={};
for(var i=0;i<f.length;i++){
  var x=f[i];
  var y=b[i];
  maps[x]=y; // maps[f[i]] = b[i]
  spam[y]=x; // spam[b[i]] = f[i]
			 // => spam[maps[i]] = i
}
exports.f=f;
exports.b=b;
exports.maps=maps;
exports.spam=spam;
