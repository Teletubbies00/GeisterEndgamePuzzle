// 取得 HTML 裡 id="board" 的元素
// 之後棋盤會全部由 JS 動態生成到這個 div 裡
const boardElement = document.getElementById("board");

// 取得右側狀態欄，也就是顯示「請選擇棋子」那個區塊
const statusElement = document.querySelector(".status");

// 這是目前測試用的棋盤資料
// 總共 36 個字元，代表 6x6 棋盤
// . 代表空格
// B 代表我方藍棋
// R 代表我方紅棋
// b 代表敵方藍棋
// r 代表敵方紅棋
// u 代表未知敵方棋子
const boardString = "R.Ruu.B.u.........................";

// 記錄目前選到哪一格
// 一開始沒有選任何格子，所以是 null
let selectedIndex = null;

// 把棋盤字元轉成畫面上顯示的符號
function getPieceSymbol(char) {
    if (char === "B") return "🔵";  // 我方藍棋
    if (char === "R") return "🔴";  // 我方紅棋
    if (char === "b") return "🔵";  // 敵方藍棋，目前先用同樣符號
    if (char === "r") return "🔴";  // 敵方紅棋，目前先用同樣符號
    if (char === "u") return "❔";  // 未知敵方棋子
    return "";                     // 空格不顯示東西
}

// 產生整個 6x6 棋盤
function renderBoard() {
    // 先清空原本棋盤內容
    // 不然每次重畫都會一直疊上去
    boardElement.innerHTML = "";

    // 6x6 棋盤共有 36 格，所以跑 0 ~ 35
    for (let i = 0; i < 36; i++) {
        // 建立一個 div，代表棋盤上的一格
        const square = document.createElement("div");

        // 把一維 index 轉成二維座標
        // 例如 i = 8
        // row = 1, col = 2
        const row = Math.floor(i / 6);
        const col = i % 6;

        // 加上 square class，讓 CSS 可以控制格子的基本樣式
        square.classList.add("square");

        // 棋盤格子交錯顏色
        // row + col 是偶數 → 淺色
        // row + col 是奇數 → 深色
        square.classList.add((row + col) % 2 === 0 ? "light" : "dark");

        // 如果這一格是目前被選中的格子，就加 selected class
        // CSS 會幫它加黃色外框
        if (i === selectedIndex) {
            square.classList.add("selected");
        }

        // 根據 boardString[i] 的字元，顯示對應棋子符號
        square.textContent = getPieceSymbol(boardString[i]);

        // 幫每一格加上點擊事件
        // 使用者點這格時，就呼叫 handleSquareClick(i)
        square.addEventListener("click", () => {
            handleSquareClick(i);
        });

        // 把這一格加入棋盤 div 裡
        boardElement.appendChild(square);
    }
}

// 處理使用者點擊棋盤格子的邏輯
function handleSquareClick(index) {
    // 取得被點擊那一格的內容
    const piece = boardString[index];

    // 如果這格不是空格，代表使用者點到棋子
    if (piece !== ".") {
        // 記錄目前選到的格子
        selectedIndex = index;

        // 更新右側狀態文字
        statusElement.textContent = `你選到了第 ${index} 格的棋子：${piece}`;
    }

    // 如果點到空格，而且前面已經有選過棋子
    else if (selectedIndex !== null) {
        // 目前只是顯示「想移動」
        // 還沒有真的改變 boardString
        statusElement.textContent = `想把第 ${selectedIndex} 格移動到第 ${index} 格`;
    }

    // 如果點到空格，但還沒選棋子
    else {
        statusElement.textContent = "這格是空的，請先選擇棋子";
    }

    // 每次點擊後重新畫棋盤
    // 這樣 selected 樣式才會更新
    renderBoard();
}

// 網頁載入後，先畫出初始棋盤
renderBoard();