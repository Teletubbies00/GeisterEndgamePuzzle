'use client'; // ➔ 告訴 Next.js 這是一張有互動功能的前端網頁

import { useState } from 'react';

export default function GeisterPuzzlePage() {
  // 1. 這是你原本的測試棋盤資料 (6x6 = 36格)
  const [boardString, setBoardString] = useState("R.Ruu.B.u.........................");
  
  // 2. 記錄目前選到哪一格 (原本的 let selectedIndex = null)
  const [selectedIndex, setSelectedIndex] = useState<number | null>(null);
  
  // 3. 記錄右側狀態欄文字 (原本的 statusElement.textContent)
  const [statusText, setStatusText] = useState("請選擇棋子");

  // 4. 把棋盤字元轉成畫面上顯示的符號 (完全繼承你原本的邏輯)
  const getPieceSymbol = (char: string) => {
    if (char === "B") return "🔵";  // 我方藍棋
    if (char === "R") return "🔴";  // 我方紅棋
    if (char === "b") return "🔵";  // 敵方藍棋
    if (char === "r") return "🔴";  // 敵方紅棋
    if (char === "u") return "❔";  // 未知敵方棋子
    return "";                     // 空格
  };

  // 5. 處理點擊棋盤格子的邏輯 (原本的 handleSquareClick)
  const handleSquareClick = (index: number) => {
    const piece = boardString[index];

    // 如果點到棋子
    if (piece !== ".") {
      setSelectedIndex(index);
      setStatusText(`你選到了第 ${index} 格的棋子：${piece}`);
    }
    // 如果點到空格，且前面有選棋子
    else if (selectedIndex !== null) {
      setStatusText(`想把第 ${selectedIndex} 格移動到第 ${index} 格 (等我們接上後端就能真的動囉！)`);
    }
    // 點到空格，但沒選棋子
    else {
      setStatusText("這格是空的，請先選擇棋子");
    }
  };

  // 6. 重來按鈕的重置邏輯
  const handleReset = () => {
    setSelectedIndex(null);
    setStatusText("請選擇棋子");
  };

  return (
    // 外層容器：支援 RWD。電腦版是橫向排列 (flex-row)，手機版寬度不夠時會自動垂直排列 (flex-col)
    <div className="min-h-screen bg-[#262421] text-white flex flex-col justify-center items-center p-4 md:p-8 font-sans">
        {/* 🎯 新增一個內部容器，控制在桌機版時的最大橫向組合寬度 */}
        <div className="w-full max-w-[960px] flex flex-col md:flex-row justify-center items-stretch gap-6 md:gap-8">

      {/* 🧩 左側棋盤區塊：用 Tailwind 的 grid 完美畫出 6x6 網格，並增加隨著螢幕等比例縮放的 RWD 功能 */}
      <div className="w-full max-w-[540px] aspect-square grid grid-cols-6 grid-rows-6 rounded-md overflow-hidden shadow-2xl">
        {/* 用陣列跑 0~35 圈，自動畫出 36 格，完全取代原本 JS 的 for 迴圈與 appendChild！ */}
        {Array.from({ length: 36 }).map((_, i) => {
          const row = Math.floor(i / 6);
          const col = i % 6;
          // 交錯棋盤色：(row + col) 是偶數 ➔ 淺色，奇數 ➔ 深色 (完全繼承你原本的樣式配置)
          const isLight = (row + col) % 2 === 0;
          const isSelected = i === selectedIndex;

          return (
            <div
              key={i}
              onClick={() => handleSquareClick(i)}
              className={`
                flex justify-center items-center text-[6vw] md:text-[42px] font-bold cursor-pointer select-none transition-all
                ${isLight ? 'bg-[#eeeed2]' : 'bg-[#769656]'}
                ${isSelected ? 'outline outline-4 outline-[#f7d154] -outline-offset-4 z-10' : ''}
              `}
            >
              {getPieceSymbol(boardString[i])}
            </div>
          );
        })}
      </div>

      {/* 📊 右側控制面板 */}
      <div className="w-full md:w-[35%] bg-[#312e2b] rounded-xl p-6 md:p-7 flex flex-col gap-5 shadow-xl">
        <div>
          <h1 className="text-3xl font-bold tracking-wide">Geister Puzzle</h1>
          <p className="text-[#c8c8c8] text-sm mt-1">5 手題 ｜ Normal</p>
        </div>

        {/* 狀態欄：會根據我們設定的 statusText 即時變更文字 */}
        <div className="bg-[#242321] p-[18px] rounded-lg min-h-[80px] text-lg flex items-center">
          {statusText}
        </div>

        {/* 按鈕功能區塊 */}
        <div className="mt-auto flex gap-3">
          <button className="flex-1 bg-[#474440] hover:bg-[#575450] text-white p-3.5 rounded-lg font-bold transition-colors">
            提示
          </button>
          <button 
            onClick={handleReset}
            className="flex-1 bg-[#81b64c] hover:bg-[#95c95a] text-white p-3.5 rounded-lg font-bold transition-colors"
          >
            重來
          </button>
          <button className="flex-1 bg-[#474440] hover:bg-[#575450] text-white p-3.5 rounded-lg font-bold transition-colors">
            下一題
          </button>
        </div>
      </div>

    </div>
    </div>
  );
}