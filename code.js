
function Check(str = "", brackets = [[]])
{
    var OpenBracketsStack = [];
    var CountOfBrackets = brackets.length;

    for(var i=0;i<str.length;i++)
    {
        var charType = -1;
        var curChar = str[i];
        var BracketID;

        for(BracketID=0;BracketID<CountOfBrackets;BracketID++)
        {
            if(curChar==brackets[BracketID][0]) {charType = 0; break;}
            if(curChar==brackets[BracketID][1]) {charType = 1; break;}
        }

        if(charType==-1) continue;

        if(charType==0)
        OpenBracketsStack.push(BracketID);
        else
        {
            if(OpenBracketsStack.length==0 || OpenBracketsStack[OpenBracketsStack.length-1]!=BracketID) return false;
            OpenBracketsStack.pop();
        }
    }

    if(OpenBracketsStack.length!=0) return false;

    return true;
}

function textBox()
{
    var input = document.getElementById("in");
    var result = Check(input.value, [['(',')'],['[',']']]);
    if(result == true)
    input.style.backgroundColor = "#66cc91";
    else
    input.style.backgroundColor = "pink";
}