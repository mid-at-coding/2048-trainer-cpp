var f=[0,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,1<<17];
var b="0123456789abcdefgh";
var maps={};
var spam={};
for(var i=0;i<f.length;i++){
  var x=f[i];
  var y=b[i];
  maps[x]=y;
  spam[y]=x;
}
exports.f=f;
exports.b=b;
exports.maps=maps;
exports.spam=spam;