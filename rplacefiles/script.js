let container = document.querySelector(".container");
let logoutButton = document.getElementById("log-out");
let clearGridButton = document.getElementById("clear-grid");
let colorButton = document.getElementById("color-input");
let eraseBtn = document.getElementById("erase-btn");
let paintBtn = document.getElementById("paint-btn");

let rows = 100;
let cols = 100;
let emptyGrid = new Array(rows).fill().map(() => new Array(cols));
let artGrid = emptyGrid;
let draw = false;
let erase = false;

let events = {
  mouse: { down: "mousedown", move: "mousemove", up: "mouseup" },
  touch: { down: "touchstart", move: "touchmove", up: "touchend" },
};

let deviceType = isTouchDevice() ? "touch" : "mouse";
let drawCount = 0;
let firstDrawTime = 0;
const MAX_DRAWINGS = 1; //Only 1 drawing per 2 seconds
const DRAWING_INTERVAL = 2000; // 2 seconds in milliseconds

function isTouchDevice() {
  try {
    document.createEvent("TouchEvent");
    return true;
  } catch (e) {
    return false;
  }
}

async function createGrid(rows, cols) {
  const storedGrid = await getGrid2();
  artGrid = storedGrid ? storedGrid : emptyGrid;

  container.innerHTML = "";
  for (let i = 0; i < rows; i++) {
    let row = document.createElement("div");
    row.classList.add("gridRow");
    for (let j = 0; j < cols; j++) {
      let col = document.createElement("div");
      col.classList.add("gridCol");
      col.setAttribute("id", `${i.toString() + "." + j.toString()}`);
      col.setAttribute("row", i);
      col.setAttribute("col", j);
      col.style.backgroundColor = artGrid[i][j] ? artGrid[i][j].color : "transparent";

      col.addEventListener(events[deviceType].down, handleMouseDown);
      col.addEventListener(events[deviceType].up, handleMouseUp);
      row.appendChild(col);
    }
    container.appendChild(row);
  }
}

function handleMouseDown() {
  if (drawCount >= MAX_DRAWINGS) {
    if (Date.now() - firstDrawTime < DRAWING_INTERVAL) {
      alert(`You can only draw ${MAX_DRAWINGS} times per 2 seconds. Please wait a moment.`);
      return;
    } else {
      drawCount = 0;
    }
  }

  if (drawCount === 0) {
    firstDrawTime = Date.now();
    localStorage.setItem("firstDrawTime", firstDrawTime);
  }

  draw = true;
  this.style.backgroundColor = erase ? "transparent" : colorButton.value;
  updateArtGrid(this);
  drawCount++;
}

function handleMouseUp() {
  draw = false;
}

function updateArtGrid(cell) {
  let cellInfo = {
    row: cell.getAttribute("row"),
    col: cell.getAttribute("col"),
    color: cell.style.backgroundColor,
  };
  artGrid[cell.getAttribute("row")][cell.getAttribute("col")] = cellInfo;
  postGrid2(artGrid);
  
}

function clearGrid() {
  deleteGrid2();
  container.innerHTML = "";
  drawCount = 0;
  localStorage.removeItem("firstDrawTime");
  createGrid(rows,cols);
}

logoutButton.addEventListener("click", logout);
clearGridButton.addEventListener("click", clearGrid);
eraseBtn.addEventListener("click", () => (erase = true));
paintBtn.addEventListener("click", () => (erase = false));

function logout() {
  localStorage.removeItem("username");
  localStorage.removeItem("password");  
  window.location.href = "login.html";
}

async function getGrid2() {
  const url = "http://localhost:8080/rplacedata/data/1";
  try {
    const response = await fetch(url);
    if (!response.ok) {
      throw new Error("Request failed");
    }
    const data = await response.json();
    return data;
  } catch (error) {
    return emptyGrid;
  }
}

function deleteGrid2() {
  const url = "http://localhost:8080/rplacedata/data/1";
  const options = {
    method: "PUT",
    body: JSON.stringify(emptyGrid),
    headers: {
      "Content-Type": "text/plain",
    },
  };

  fetch(url, options)
    .then((response) => {
      if (!response.ok) {
        throw new Error("Request failed");
      }
    })
    .catch((error) => {
    });
}

function postGrid2(grid) {
  var username = localStorage.getItem('username');
  var password = localStorage.getItem('password');
  if (username == null || password == null)
  {
    window.alert('You are not logged in!');
    window.location.href = "login.html";
  }
  let url = "http://localhost:8080/rplaceuser/" + username + "=" + password;
  const options = {
    method: "PUT",
    body: JSON.stringify(grid),
    headers: {
      "Content-Type": "text/plain",
    },
  };

  // auth
  fetch(url, options)
    .then((response) => {
      if (!response.ok) {
        window.alert('You are not logged in!');
        window.location.href = "login.html";
      }
      else {
        url = "http://localhost:8080/rplacedata/data/1";
        fetch(url, options)
        .then((response) => {
          if (!response.ok) {
            throw new Error("Request failed");
          }
          else {
          }
        })
        .catch((error) => {
        });
      }
    })
    .catch((error) => {
    });
  
}

async function fetchDataAndCreateGrid() {
  await createGrid(rows, cols);
  setInterval(async () => {
    const storedGrid = await getGrid2();
    artGrid = storedGrid;
    updateGridUI();
  }, 300); 
}

function updateGridUI() {
  const gridCols = document.getElementsByClassName("gridCol");
  for (let i = 0; i < gridCols.length; i++) {
    const gridCol = gridCols[i];
    const row = gridCol.getAttribute("row");
    const col = gridCol.getAttribute("col");
    gridCol.style.backgroundColor = artGrid[row][col] ? artGrid[row][col].color : "transparent";
  }
}

fetchDataAndCreateGrid();
