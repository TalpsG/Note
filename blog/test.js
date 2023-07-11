function Click(){
    answer = prompt("hello","123")
    console.log(answer)
}
function cal() {
    let a = parseFloat(document.getElementById("num1").value)
    let b = parseFloat(document.getElementById("num2").value)
    let op = parseInt(document.getElementById("operator").value)
    switch (op) {
        case 1:
            answer = a+b
            break;
        case 2:
            answer = a-b
            break;
        case 3:
            answer = a*b
            break;
        case 4:
            answer = a/b
            break;
    }
    document.getElementById("answer").innerHTML = answer 
}