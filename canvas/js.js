const canvas = document.getElementById('canvas');
const ctx = canvas.getContext('2d');

const minSide = Math.min(document.body.clientWidth,document.body.clientHeight);
canvas.width = minSide; 
canvas.height = minSide; 

const rectCount = 100;
const rectSize = minSide/rectCount;

function drawPixel(pox, poy, r,g,b, sD)
{
    ctx.fillStyle = `rgb(${r},${g},${b})`;
    ctx.fillRect(parseInt(pox)*rectSize, parseInt(poy)*rectSize, rectSize, rectSize);
}

function object2d()
{
    this.x = 0;
    this.y = 0;
    this.speedY = 0;
    this.speedX = 0;
}

let obj = new object2d();
obj.x = rectCount/2;
obj.speedX=Math.random()*3-1.5;

const sx = 0.07;

let path = [[0,0]];
const length = 15;

setInterval(()=>{
    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    path.forEach((elem,i)=>{
        let r = 255/length*i;
        drawPixel(elem[0],elem[1],r,r*0.3,0);
    });

    if(obj.x+obj.speedX>rectCount || obj.x+obj.speedX<0)
        obj.speedX = -obj.speedX;
    else
        obj.x+=obj.speedX;

    if(obj.y+obj.speedY<rectCount)
    {
        obj.y+=obj.speedY;
        obj.speedY+=0.01;
    } else
    {
        obj.speedY=-obj.speedY*0.8;
        if(obj.speedY<0)
            if(Math.abs(obj.speedX)<sx) obj.speedX = 0;
            else
            obj.speedX+=Math.sign(obj.speedX)*-sx;
    }
    
    path.push([obj.x,obj.y]);
    if(path.length>=length)
    path.shift();
},10);