
function buildWeights(inputElements, checkElements)
{
    let outArr = [];

    let goodOnes = checkElements.reduce((acc,curr)=>acc+curr);
    let badOnes =  (checkElements.length - goodOnes); 

    let min = Math.min(inputElements.length, checkElements.length);
    for(let i=0;i<min;i++)
    if(inputElements[i].checked)
    outArr.push(checkElements[i]==inputElements[i].checked?1/goodOnes:-1/badOnes);
    else
    outArr.push(0);

    return outArr;
}

document.getElementById('sub').addEventListener("click", ()=>{
    const firstVars = document.getElementsByName('first');
    const secondVars = document.getElementsByName('second');
    const thirdVar = document.getElementById('third');

    const firstChecker = [0,1,0,0,0];
    const secondChecker = [1,1,1,0];
    let score = 0;

    let firstWeights = buildWeights(firstVars, firstChecker);
    let secondWeights = buildWeights(secondVars, secondChecker);
    let thirdWeight = thirdVar.value=='5';

    let coloring = (vars, weights, checker) =>{
        let BlockBorderClr = 0;
        vars.forEach((elem, i)=>{
        if(checker[i]) elem.parentElement.style.background = 'yellow';

        let temp = weights[i];
        if(!temp) return;
        if(temp>0)
        {
            elem.parentElement.style.background = 'green';
            BlockBorderClr|=1;
        } else
        {
            elem.parentElement.style.background = 'red';
            BlockBorderClr|=2;
        }
        });
        const colors = ['gray', 'green', 'red', 'yellow'];
        vars[0].parentElement.parentElement.parentElement.style.border = '3px solid '+colors[BlockBorderClr];
    };

    coloring(firstVars, firstWeights, firstChecker);
    coloring(secondVars, secondWeights, secondChecker);

    thirdVar.parentElement.style.border = '3px solid '+(thirdWeight?'green':'red');

    score+=Math.max(firstWeights.reduce((acc,curr)=>acc+curr),0);
    score+=Math.max(secondWeights.reduce((acc,curr)=>acc+curr),0);
    score+=thirdWeight;

    document.getElementById("good").value = score;
    document.getElementById("bad").value = 3-score;

    if(score>2.99)
        document.getElementById("good").style.background="green";
    else if(score == 0)
        document.getElementById("good").style.background="red";

    let allInputs = document.getElementsByTagName("input");
    for(let i = 0;i<allInputs.length;i++)
    allInputs[i].disabled = true;
    thirdVar.disabled = true;
});

document.getElementById('ret').addEventListener("click", ()=>{
location.reload();
});