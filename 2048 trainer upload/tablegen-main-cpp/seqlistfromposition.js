// PURPOSE
// gen all legal moves starting at some position

var config=require("./config.js").config;
var movefile=config.movefile;
var movedirfile=config.movedirfile;
//0 1 2 3
//  4 5 6
//  7
//  8
var moveleft=require(movedirfile).moveleft;
var moveright=require(movedirfile).moveright;
var moveup=require(movedirfile).moveup;
var movedown=require(movedirfile).movedown;
var maps=require("./maps.js").maps;
//console.log(sum);
var addpool=function(item,pool,Pool){
  if(!Pool[item]){   // if item is not in the pool(?)
    Pool[item]=1;    // add it to the pool
    pool.push(item); // add it to the pool
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
  for(var i=0;i<moved.length;i++){ 	// loop through the legal moves
    var item=moved[i]; 
    addpool(item,pool,Pool); 		// add them to the pool
  }
};
var addn=function(item,pool,Pool,n){
  for(var i=0;i<item.length;i++){ 	// loop through this board's spaces(?)
    if(!item[i]){   				// if a certain spot doesn't have a tile
      var newitem=item.slice();		// make some new item, set equal to input board
      newitem[i]=n;  				// set the empty space to the input n
      addpool(newitem,pool,Pool);   // add this board to the pool
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
var l=a.length; // size of the queue / pool(?)
while(l){
  console.log("sum="+sum);
  console.log("Pool size="+l);
  console.log("Positions logged="+positions);
  var str=a.map(x=>x.map(y=>maps[y]).join("")).join("\n"); // ok man
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
