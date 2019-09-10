uses graphabc;

type
  
  point = record
    x, y: integer;
  end;
  
  TDirs = (North, East, South, West);
  tredraw = (Full, OnlyHero);

const
  w = 10;
  h = 10;
  ww = 700;
  hh = 700;
  linew=1;

var
  stack: Stack<point> := new Stack<point>();
  
  RatMap: array[0..w - 1, 0..h - 1] of boolean;
  currentErasePointPos: point;
  hero:point;
  
  wallFieldV: array[0..w, 0..h] of boolean;
  wallFieldH: array[0..w, 0..h] of boolean;

  background:picture :=new Picture(ww,hh);

procedure ClearArrays();
begin
for var i := 0 to h do
    for var j := 0 to w do
    begin
    wallFieldV[j,i]:=false;
    wallFieldH[j,i]:=false;
    end;

for var i := 0 to h-1 do
    for var j := 0 to w-1 do
      RatMap[j,i]:=false;
end;

function CanGo(pos: point; dir: Tdirs): boolean;
begin
  result := false;
  
  if(pos.x = 0) and (dir = TDirs.West) 
  or (pos.y = 0) and (dir = TDirs.North) then 
    exit; 
  
  if(pos.x = w - 1) and (dir = TDirs.East)
  or (pos.y = h - 1) and (dir = TDirs.South) then 
    exit;
  
  if(dir = tdirs.North) and (RatMap[pos.x, pos.y - 1]) then exit;
  if(dir = tdirs.East) and (RatMap[pos.x + 1, pos.y]) then exit;
  if(dir = tdirs.South) and (RatMap[pos.x, pos.y + 1]) then exit;
  if(dir = tdirs.West) and (RatMap[pos.x - 1, pos.y]) then exit;
  
  result := true;
end;

procedure drawField(render:tredraw);
begin
  var rectS := window.Height / w;
  
  pen.Width := lineW; 
  
  if(render=tredraw.Full) then
  begin
  background.Clear();
  for var i := 0 to h do
    for var j := 0 to w do
    begin
      var topleftx := round(j * rectS);
      var toplefty := round(i * rectS);
      
      if(not wallFieldV[j, i]) then
        background.line(topleftx, toplefty, topleftx, round(toplefty + rectS));
      
      if(not wallFieldH[j, i]) then
        background.line(topleftx, toplefty, round(topleftx + rectS), toplefty);
    end;
    end;
    var topleftx := round(hero.x * rectS);
    var toplefty := round(hero.y * rectS);
    
    background.Draw(0,0);
    
    brush.Color:=clred;
    fillrect(topleftx, toplefty,
    round(topleftx+rectS),round(toplefty+rectS));
end;

procedure MoveDown();
begin
  currentErasePointPos.y += 1;
  WallFieldH[currentErasePointPos.x, currentErasePointPos.y] := true;
end;

procedure MoveUp();
begin
  WallFieldH[currentErasePointPos.x, currentErasePointPos.y] := true;
  currentErasePointPos.y -= 1;
end;

procedure MoveLeft();
begin
  WallFieldV[currentErasePointPos.x, currentErasePointPos.y] := true;
  currentErasePointPos.x -= 1;
end;

procedure MoveRight();
begin
  currentErasePointPos.x += 1;
  WallFieldV[currentErasePointPos.x, currentErasePointPos.y] := true;
end;

procedure BuildMaze;
begin
  var cant_go_anymore: boolean := false;
  var cell_counter: integer := 1;
  
  while true do
  begin
    var directions: array[0..3] of byte := (0, 1, 2, 3);
    
    for var i := 1 to 5 do
      swap(directions[random(0, 3)], directions[random(0, 3)]);
    
    cant_go_anymore := true;
    var curDir: tdirs;
    
    for var i := 0 to 3 do
      if(CanGo(currentErasePointPos, Tdirs(directions[i]))) then
      begin
        cant_go_anymore := false;
        curDir := Tdirs(directions[i]);
      end;
    
    if(not cant_go_anymore) then
    begin
      stack.Push(currentErasePointPos);
      RatMap[currentErasePointPos.x, currentErasePointPos.y] := true;
      inc(cell_counter);
      case curDir of
        tdirs.North: MoveUp;
        tdirs.West: MoveLeft;
        tdirs.South: MoveDown;
        tdirs.East: MoveRight;
      end;
    end else
    begin
      RatMap[currentErasePointPos.x, currentErasePointPos.y] := true;
      currentErasePointPos := stack.Pop();
      if(cell_counter >= w * h) then exit;
    end;
    

  end;
  
end;

procedure kd(key: integer);
begin
case key of 
VK_W:if(hero.y<>0)and(WallFieldH[hero.x,hero.y])then dec(hero.y);
VK_S:if(hero.y<>h-1)and(WallFieldH[hero.x,hero.y+1])then inc(hero.y);
VK_A:if(hero.x<>0)and(WallFieldV[hero.x,hero.y])then dec(hero.x);
VK_D:if(hero.x<>w-1)and(WallFieldV[hero.x+1,hero.y])then inc(hero.x);
end;

if(key=vk_r)or(hero.x=w-1)and(hero.y=h-1) then
begin
ClearArrays();
BuildMaze();
hero.x:=0;
hero.y:=0;
DrawField(tredraw.Full);
end;

clearwindow();
DrawField(tredraw.OnlyHero);
redraw;
end;

begin
  setwindowsize(ww, hh);
  setwindowtitle('maze');
  graphabc.SetConsoleIO;
  LockDrawing;
  onkeydown := kd;
end.