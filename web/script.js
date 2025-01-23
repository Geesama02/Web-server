let obj = [
    {"name" : "cr7", "number": 7},
    {"name" : "messi", "number": 10}
]

function createPlayer(newElem) {
    return new Promise((resolve, reject) => {
        setTimeout(() => {
            obj.push(newElem);
            resolve();
        }, 2000);
    });
}

function showPlayers() {
    let output = "";
    obj.forEach((elem) => {
        output += `<li>${elem.name}</li> <p>${elem.number}</p>`;
    })
    document.body.innerHTML = output;
    console.log("Promise Success!!");
}

function rejectedProm() {
    console.log("Error in Promise!!");
}

// showPlayers();
createPlayer({"name": "marouan", "number": 9}).then(showPlayers, rejectedProm);