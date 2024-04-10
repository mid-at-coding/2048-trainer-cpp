// PURPOSE
// gen all legal moves starting at some position

var config=require("./config.js").config;
var movefile=config.movefile;
var movedirfile=config.movedirfile;
var move=require(movefile).move;
var pair=require(movefile).pair;
//0 1 2 3
//  4 5 6
//  7
//  8
var moveleft=require(movedirfile).moveleft;
var moveright=require(movedirfile).moveright;
var moveup=require(movedirfile).moveup;
var movedown=require(movedirfile).movedown;
var maps=require("./maps.js").maps;
var spam=require("./maps.js").spam;
//console.log(sum);
var addpool=function(item,pool,Pool){
  if(!Pool[item]){
    Pool[item]=1;
    pool.push(item);
  }
};
var satisfied=require(config.satisfiedfile).satisfied;
var add=function(item,pool,Pool){
  if(satisfied(item)){//Do we really need to examine this further?
    return;
  }
  var left=item.slice();
  var right=item.slice();
  var up=item.slice();
  var down=item.slice();
  var moved=[];
  if(moveleft(left)){
    moved.push(left);
  }
  if(moveright(right)){
    moved.push(right);
  }
  if(moveup(up)){
    moved.push(up);
  }
  if(movedown(down)){
    moved.push(down);
  }
  for(var i=0;i<moved.length;i++){
    var item=moved[i];
    addpool(item,pool,Pool);
  }
};
var addn=function(item,pool,Pool,n){
  for(var i=0;i<item.length;i++){
    if(!item[i]){
      var newitem=item.slice();
      newitem[i]=n;
      addpool(newitem,pool,Pool);
    }
  }
}
var fs=require("fs");
var sum=0;
var start=[2,0,0,0,0,0,0,0,4,8];
var a=[start];
var A={};
var b=[];
var B={};
var c=[];
var C={};
A[start]=1;
start.map(x=>sum+=x);

var positions=0;
var l=a.length;
while(l){
  console.log("sum="+sum);
  console.log("Pool size="+l);
  console.log("Positions logged="+positions);
  var str=a.map(x=>x.map(y=>maps[y]).join("")).join("\n");
  fs.writeFileSync("./"+config.positionsdir+"/"+sum+".txt",str);
  var pool=[];
  var Pool={};
  a.map(x=>add(x,pool,Pool));
  //console.log(pool);
  pool.map(x=>addn(x,b,B,2));
  pool.map(x=>addn(x,c,C,4));
  a=b;
  A=B;
  b=c;
  B=C;
  c=[];
  C={};
  sum+=2;
  l=a.length;
  positions+=l;
} 
