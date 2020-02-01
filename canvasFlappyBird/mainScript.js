const canvas = document.getElementById("canvas");
const ctx = canvas.getContext('2d');

canvas.height = document.body.clientHeight;
canvas.width = canvas.height;

let mouseDown = false;

canvas.onmousedown = ()=>{mouseDown = true;}
canvas.onmouseup = ()=>{mouseDown = false;}

//Pics
const backgroundPic = new Image();
backgroundPic.src = "Sprites/background-day.png";

const floorPic = new Image();
floorPic.src = "Sprites/base.png";

const birdPicAnimated = new Image();
birdPicAnimated.src = "Sprites/birdSpread.png";

let BirdW;
let BirdH; 

//SubCanvases
const backgroundCanvas = document.createElement("canvas");
const floorCanvas = document.createElement("canvas");

backgroundPic.onload = ()=>{
    backgroundCanvas.width = canvas.width;
    backgroundCanvas.height = canvas.height;

    const tempCtx = backgroundCanvas.getContext('2d');
    const w = Math.floor(canvas.height/backgroundPic.height*backgroundPic.width);
    backgroundPic.w = w;
    backgroundPic.h = canvas.height;

    for(let i = 0;i*backgroundPic.w<canvas.width;i++)
    tempCtx.drawImage(backgroundPic, i*backgroundPic.w, 0, backgroundPic.w, backgroundPic.h);
};

floorPic.onload = ()=>{
    floorCanvas.width = canvas.width+floorPic.width;
    floorCanvas.height = canvas.height;

    const tempCtx = floorCanvas.getContext('2d');
    const neededHeight = Math.floor(canvas.height/10);
    const w = Math.floor(neededHeight/floorPic.height*floorPic.width);
    floorPic.w = w;
    floorPic.h = neededHeight;
    for(let i = 0;i*floorPic.w<canvas.width+floorPic.w;i++)
    tempCtx.drawImage(floorPic, i*floorPic.w, canvas.height-floorPic.h, floorPic.w, floorPic.h);
};

birdPicAnimated.onload = ()=>{
    const neededHeight = Math.floor(canvas.height/5);
    const w = Math.floor(neededHeight/birdPicAnimated.height*birdPicAnimated.width);
    birdPicAnimated.w = w;
    birdPicAnimated.h = neededHeight;

    birdW = w;
    birdH = Math.floor(neededHeight/3);
};

function drawFloorLayer(offset)
{
    offset=offset%floorPic.w;
    ctx.drawImage(floorCanvas,offset,0);
}

function drawBird(x,y,animId, rot){
    x-=birdPicAnimated.w/2;
    y-=birdPicAnimated.h/2;
    const w = birdW;
    const h = birdH;
    const dy = 24*animId;
    const cx = x+w/2;
    const cy = y+h/2;
    ctx.save();
    ctx.translate(cx, cy);
    ctx.rotate(rot);
    ctx.translate(-cx, -cy);
    ctx.drawImage(birdPicAnimated,0,dy,34,24,x,y,w,h);
    ctx.restore();
}

const fallAcceleration = canvas.height/1000;
const fallingSpeedLimit = canvas.height/50;
const flapValue = canvas.height/50;

let temp = 0;
let vSpeed = 0;
let yPos = canvas.height/2;

let frameCounter = 0;
setInterval(() => {
   ctx.drawImage(backgroundCanvas,0,0);
   drawFloorLayer(temp-=3);
   
   drawBird(canvas.width/2,yPos,Math.floor(-temp/20)%3,(Math.PI / 180) * vSpeed*4);

   if(mouseDown){
       vSpeed=-flapValue;
       mouseDown = false; 
   }

   if(fallAcceleration+vSpeed>fallingSpeedLimit)
   vSpeed = fallingSpeedLimit;
   else
   vSpeed+=fallAcceleration;

   if(yPos+vSpeed-birdH<canvas.height-floorPic.h)
   yPos+=vSpeed;

   frameCounter++;
}, 16);

setInterval(() => {
    document.title = frameCounter+" fps";
    frameCounter = 0;
}, 1000);

