var config=require("./config.js").config;
var movefile=config.movefile;
var movedirfile=config.movedirfile;
var move=require(movefile).move;
var pair=require(movefile).pair;
var moveleft=require(movedirfile).moveleft;
var moveright=require(movedirfile).moveright;
var moveup=require(movedirfile).moveup;
var movedown=require(movedirfile).movedown;
var maps=require("./maps.js").maps;
var spam=require("./maps.js").spam;
var addrandomtile=function(board){
  var blanks=[];
  for(var i=0;i<board.length;i++){
    board[i]?null:blanks.push(i);
  }
  var p=Math.floor(Math.random()*blanks.length);
  board[blanks[p]]=Math.random()<0.1?4:2;
};
var moveall=function(board,direction){
  if(direction==="L"){
    moveleft(board)?addrandomtile(board):null;
  }else if(direction==="R"){
    moveright(board)?addrandomtile(board):null;
  }else if(direction==="U"){
    moveup(board)?addrandomtile(board):null;
  }else if(direction==="D"){
    movedown(board)?addrandomtile(board):null;
  }
};
//var board=[2,2,0,0,0,0,0,0];
var board=[8,4,4,8,4,32,64,128,0,256,2];
var fs=require("fs");
var f=require("./maps.js").f;
var g=["_   ","2   ","4   ","8   ","16  ","32  ","64  ","128 ","256 ","512 ","1024","2048"];
var b="0123456789ab";
var maps={};
var spam={};
for(var i=0;i<f.length;i++){
  var F=f[i];
  maps[F]=b[i];
  spam[F]=g[i];
}
var getmove=function(board){
  var s=0;
  board.map(x=>(s+=x));
  var str=board.map(x=>maps[x]).join("");
  var table=fs.readFileSync("./"+config.tablesdir+"/"+s+".txt","utf-8").split("\n").map(x=>x.split(" "));
  var row;
  for(var i=0;i<table.length;i++){
    row=table[i];
    if(row[0]===str){
      break;
    }
  }
  var prob=row[1];
  var i=3;
  while(row[i]!==prob){
    i+=2;
    if(i>=row.length){
      process.exit();
    }
  }
  //console.log(row);
  return row[i-1];
};
var map=function(y,board){
  if(y===-1){
    return "    ";
  }else{
    return board[y];
  }
};
var shape=[[0,1,2,3],[4,5,6,7],[8,9],[10]];
var printboard=function(board){
  board=board.map(x=>spam[x]);
  var s=shape.map(x=>x.map(y=>map(y,board))).map(x=>x.join(" ")).join("\n");
  console.log(s);
};
printboard(board);
//console.log(getmove(board));
while(true){
  var m=getmove(board);
  console.log(m);
  moveall(board,m);
  printboard(board);
}
  