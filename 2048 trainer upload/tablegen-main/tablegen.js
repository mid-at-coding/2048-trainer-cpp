// PURPOSE
// ??

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
// this function turns a given string into a board
// a board seems to be a stack of numbers.
var str2board=function(str){
  var board=[];
  for(var i=0;i<str.length;i++){
    board.push(spam[str[i]]); // TODO: change to log
  }
  return board;
};
var fs=require("fs");
var s0=config.s0;
var s1=config.s1;
// TODO: figure out wtf s0 means, why it's being incremented by two, and why it ends at s0+4
for(var i=s0+2;i<=s0+4;i+=2){
  var positions=fs.readFileSync("./"+positionsdir+"/"+i+".txt","utf-8").split("\n");
  // get a position (what the fuck is a position??) from some 'positionsdir' which is specified in config.txt and is by default set to 'positionstest'
  // it is worth noting ./positionstest is not a directory that exists(indeed, there is no directory named positionstest in this repo)
  if(positions[positions.length-1]===""){ // if the last line of the raw data for this position is empty, remove it
    positions.pop();
  }
  positions=positions.map(x=>x+" "+1*satisfied(str2board(x))); // turn 'positions' into a map ...
  // where every line is mapped onto a binary value that's true if str2board(x)[9] == 1024 (??)
  fs.writeFileSync("./"+tablesdir+"/"+i+".txt",positions.join("\n"));
  // write to some tablesdir, specified in config.txt. like the positionsdir, this is not a directory that exists by default
} // ???? come back to this - what the fuck is this satisfied function(and why must it explicitly be multiplied by one??)
  // ^^ after digging deeper, the answer is still "what the fuck"
  // TODO: find out the purpose of the satisfied function, probably some obscure tech

// TODO: find out what the args to this mean
var evalprob=function(moved,v,w){//Expectimax
  var empty=0; // ??
  var prob2=0; // ??
  var prob4=0; // ??
  for(var i=0;i<moved.length;i++){ // come back to this: what the fuck is moved
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
// the motherfuck unlimited. dig in folks
var solve=function(s){
  var t=s+2;
  var u=s+4;
  var a=fs.readFileSync("./"+positionsdir+"/"+s+".txt","utf-8").split("\n"); 
  // get a position from positionsdir, enumerated by s, an arg
  // TODO: find the significance of s
  if(a[a.length-1]===""){
    // remove the last line if it's empty
    a.pop();
  }
  var d=a.map(x=>str2board(x));
  // convert raw data from a into multiple boards
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
    // go through all the boards from a(now in d)
    var board=d[i];
	// set board equal to the current board from d
    if(satisfied(board)){
	  // if the ninth element of this board is equal to 1024
      //u.push([a[i],1]);
      u+=a[i]+" 1\n"; // TODO: what the fuck does this mean
      continue;
    }
    var probs=[];
    var bestprob=0;
    var moved=board.slice(); // ??
    if(moveleft(moved)){//Try each direction
	  // TODO: what is moveleft: movedir.js
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
