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
var satisfied=require(config.satisfiedfile).satisfied;
var positionsdir=config.positionsdir;
var tablesdir=config.tablesdir;
var str2board=function(str){
  var board=[];
  for(var i=0;i<str.length;i++){
    board.push(spam[str[i]]);
  }
  return board;
};
var fs=require("fs");
var s0=config.s0;
var s1=config.s1;
for(var i=s0+2;i<=s0+4;i+=2){
  var positions=fs.readFileSync("./"+positionsdir+"/"+i+".txt","utf-8").split("\n");
  if(positions[positions.length-1]===""){
    positions.pop();
  }
  positions=positions.map(x=>x+" "+1*satisfied(str2board(x)));
  fs.writeFileSync("./"+tablesdir+"/"+i+".txt",positions.join("\n"));
}
var evalprob=function(moved,v,w){//Expectimax
  var empty=0;
  var prob2=0;
  var prob4=0;
  for(var i=0;i<moved.length;i++){
    if(moved[i]){
      continue;
    }
    empty+=1;
    moved[i]=2;
    prob2+=v[moved];
    moved[i]=4;
    prob4+=w[moved];
    moved[i]=0;
  }
  return (9*prob2+1*prob4)/10/empty;
};
var n4;
var solve=function(s){
  var t=s+2;
  var u=s+4;
  var a=fs.readFileSync("./"+positionsdir+"/"+s+".txt","utf-8").split("\n");
  if(a[a.length-1]===""){
    a.pop();
  }
  var d=a.map(x=>str2board(x));
  var v={};
  var w={};
  if(n4){
    w=n4;
  }else{
    console.log("Processing "+u+".txt...");
    var c=fs.readFileSync("./"+tablesdir+"/"+u+".txt","utf-8").split("\n").map(x=>x.split(" ").slice(0,2)).map(x=>[str2board(x[0]),1*x[1]]);
    c.map(x=>w[x[0]]=x[1]);
    c=[];
  }
  console.log("Processing "+t+".txt...");
  var b=fs.readFileSync("./"+tablesdir+"/"+t+".txt","utf-8").split("\n").map(x=>x.split(" ").slice(0,2)).map(x=>[str2board(x[0]),1*x[1]]);
  b.map(x=>v[x[0]]=x[1]);
  n4=v;//No need to process this twice
  b=[];
  console.log("Solving s="+s);
  var u="";
  for(var i=0;i<d.length;i++){
    var board=d[i];
    if(satisfied(board)){
      //u.push([a[i],1]);
      u+=a[i]+" 1\n";
      continue;
    }
    var probs=[];
    var bestprob=0;
    var moved=board.slice();
    if(moveleft(moved)){//Try each direction
      probs.push("L");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    moved=board.slice();
    if(moveright(moved)){
      probs.push("R");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    moved=board.slice();
    if(moveup(moved)){
      probs.push("U");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    moved=board.slice();
    if(movedown(moved)){
      probs.push("D");
      var prob=evalprob(moved,v,w);
      probs.push(prob);
      bestprob=Math.max(prob,bestprob);
    }
    probs=[a[i],bestprob].concat(probs);
    u+=probs.join(" ")+"\n";
  }
  //u=u.join("\n");
  fs.writeFileSync("./"+tablesdir+"/"+s+".txt",u);
};
for(var i=s0;i>=s1;i-=2){
  solve(i);
}