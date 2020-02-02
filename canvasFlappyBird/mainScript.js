const canvas = document.getElementById("canvas");
const ctx = canvas.getContext('2d');

const min = Math.min(document.body.clientHeight, document.body.clientWidth);
canvas.height = min;
canvas.width = min;

let mouseDown = false;

canvas.addEventListener('pointerdown',()=>{mouseDown = true;});

const FlappyFont = new FontFace('Flappy', 'url(FlappyBirdRegular.ttf)');

const backgroundPic = new Image();
backgroundPic.src = "Sprites/background-day.png";

const floorPic = new Image();
floorPic.src = "Sprites/base.png";

const birdPicAnimated = new Image();
birdPicAnimated.src = "Sprites/birdSpread.png";

const pipePic = new Image();
pipePic.src = "Sprites/pipe-green.png";

const pipePic_rev = new Image();
pipePic_rev.src = "Sprites/pipe-green-rev.png";

const backgroundCanvas = document.createElement("canvas");
const floorCanvas = document.createElement("canvas");

let birdW;
let birdH; 
const countOfLoaders = 6;
let countOfLeftToLoad = countOfLoaders; 

FlappyFont.load().then(()=>{
    document.fonts.add(FlappyFont);
    countOfLeftToLoad--;
});

backgroundPic.onload = ()=>{
    backgroundCanvas.width = canvas.width;
    backgroundCanvas.height = canvas.height;

    const tempCtx = backgroundCanvas.getContext('2d');
    const w = Math.floor(canvas.height/backgroundPic.height*backgroundPic.width);
    backgroundPic.w = w;
    backgroundPic.h = canvas.height;

    for(let i = 0;i*backgroundPic.w<canvas.width;i++)
    tempCtx.drawImage(backgroundPic, i*backgroundPic.w, 0, backgroundPic.w, backgroundPic.h);
    countOfLeftToLoad--;
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
    countOfLeftToLoad--;
};

birdPicAnimated.onload = ()=>{
    const neededHeight = Math.floor(canvas.height/5);
    const w = Math.floor(neededHeight/birdPicAnimated.height*birdPicAnimated.width);
    birdPicAnimated.w = w;
    birdPicAnimated.h = neededHeight;

    birdW = w;
    birdH = Math.floor(neededHeight/3);
    countOfLeftToLoad--;
};

pipePic.onload = ()=>{
    const neededWidth = canvas.height/7;
    const h = Math.floor(neededWidth/pipePic.width*pipePic.height);
    pipePic.w = Math.floor(neededWidth);  
    pipePic.h = h;
    countOfLeftToLoad--;
};

pipePic_rev.onload = ()=>{
    const neededWidth = canvas.height/7;
    const h = Math.floor(neededWidth/pipePic_rev.width*pipePic_rev.height);
    pipePic_rev.w = Math.floor(neededWidth);  
    pipePic_rev.h = h;
    countOfLeftToLoad--;
};

function drawFloorLayer(offset)
{
    offset=offset%floorPic.w;
    ctx.drawImage(floorCanvas,Math.floor(offset),0);
}

function drawBird(x,y,animId, rot){
    x-=birdW/2;
    y-=birdH/2;
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


function Start()
{
const fallAcceleration = canvas.height* 1/1000;
const fallingSpeedLimit = canvas.height * 1/50;
const flapValue = canvas.height * 1/50;
const floorSpeed = canvas.height * 1/200;
const tubeCrack = canvas.height*0.30;

let basePosition = 0;
let vSpeed = 0;
let yPos = canvas.height/2;
let BirdInZone = [];
let gameover = false;
let score = 0;

const pipeCount = 2;
const pipeInterval = canvas.width/pipeCount+pipePic.w/2;

let pipeArr = [];

for(let i=0;i<pipeCount;i++)
    pipeArr.push([canvas.width+i*pipeInterval, (canvas.height-floorPic.h-tubeCrack*1.40)*Math.random()+tubeCrack*0.70]);

let int = setInterval(() => {
   ctx.drawImage(backgroundCanvas,0,0);

   pipeArr.forEach((val,i)=>{
    if(val[0]<(canvas.width+birdW)/2 && val[0]+pipePic.w>(canvas.width-birdW)/2) 
    {   
        BirdInZone[i] = true;
        const kTop = val[1]-tubeCrack/2;
        const kBottom = val[1]+tubeCrack/2;
        if(yPos-birdH/2<kTop || yPos+birdH/2>kBottom) 
            gameover = true;
    } else if(BirdInZone[i]) {score++; BirdInZone[i] = false;}

    ctx.drawImage(pipePic_rev, Math.floor(val[0]),Math.floor(val[1]-tubeCrack/2-pipePic_rev.h), pipePic_rev.w, pipePic_rev.h);
    ctx.drawImage(pipePic, Math.floor(val[0]),Math.floor(val[1]+tubeCrack/2), pipePic.w, pipePic.h);

    val[0]-=floorSpeed*!gameover;
    if(val[0]+pipePic.w<0) {
        val[1] = (canvas.height-floorPic.h-tubeCrack*1.40)*Math.random()+tubeCrack*0.70;
        val[0] = canvas.width;
        }
   });

   drawFloorLayer(basePosition-=floorSpeed*!gameover);
   
   let angle = (Math.PI / 180) *(vSpeed/fallingSpeedLimit);
   angle *= vSpeed<0?30:70;
   drawBird(canvas.width/2,yPos,Math.floor(-basePosition/20)%3,angle);

   ctx.font = Math.floor(canvas.height/5)+"px Flappy";
   ctx.fillStyle = "white";
   ctx.textAlign = "center";
   ctx.fillText(score, canvas.width/2, canvas.height/5); 

   if(mouseDown && !gameover){
       vSpeed=-flapValue;
       mouseDown = false; 
   }

   if(fallAcceleration+vSpeed>fallingSpeedLimit)
   vSpeed = fallingSpeedLimit;
   else
   vSpeed+=fallAcceleration;

   if(yPos+vSpeed-birdH/2<canvas.height-floorPic.h)
   yPos+=vSpeed;
   else
   {
    clearInterval(int);
    setTimeout(() => {
        Start();
    }, 1000);
   }

}, 1000/60);

}

let int = setInterval(() => {
    if(!countOfLeftToLoad)
    {
        clearInterval(int);
        Start();
    }
}, 33);
