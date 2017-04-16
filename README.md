# RFID-資安週DEMO操作手冊

- 開機後，請按照Code Comment的方式接線
```
White Card UID (Used to pass):14 0D 56 45 
Blue magnetic buckle UID (Used to error):23 6D FB A4

Purple magnetic buckle origin UID: 0B EA 3C BC

sccot RFID:0B EA 3C BC 
RFID Module PIN:
    Reset Pin (RST) :9 - Orange
    SS (SDA) :10 - Red
    MOSI : 11 - Brown
    MISO : 12 - Black
    SCK : 13 - White 
    GND : GND - gray
    3.3V : 3.3V - purple
Servo:
    GND-GND(Brown) VCC-VCC(Red) 3-Signal(Orange)
close LED: 6-GND(Black) 7-VCC (White)
Open LED: 4-GND (Purple) 5-VCC (Orange)
Switch: 8-VCC+pullup 0-GND
```
[Github Source Code](https://github.com/alieonsido/RFID_-Information-security-week)
## 環境安裝方法:
1. 先安裝Arduino IDE
2. 將上面的Source code安裝進去，如果已安裝，則跳過此步驟
3. 將板子用USB插入電腦
4. 點開工具列 > 工具 > 序列埠，選取你所接到的COM腳
5. 點下左上方的燒錄按鈕(箭頭)
6. 打開右上方的串列埠(放大鏡圖案)，會有提示訊息跳出

## 操作方式
- 擁有讀 & 寫模式，利用開關來切換，當每次重開機時都會告知你現在的模式。
    - 讀：當Reader讀取正確的卡片時，門鎖會打開，並有提示燈號。
        - 白色的卡片：PASS CARD
        - 藍色的磁扣：error CARD
        - 紫色的磁扣：自定義。
    - 寫：可以使用未鎖UID的磁扣來寫入新的UID。在Source code的`#define NEW_UID {0xDE, 0xAD, 0xBE, 0xEF} //DEFINE NEW UID HERE . `在Comment的Header，有提示Pass的UID。
        - 注意：更改新的UID後，要重新燒錄程式，再來將尚未鎖UID的磁扣放入Reader，才可以把新的UID刷進去，**請等資料Dump完畢以後再將磁扣移開。**

## 資安週當週注意事項
- 請不要把Servo接錯，請不要把Servo接錯，請不要把Servo接錯。
    - 很重要所以說三次。
- RFID的接線方式我有排列過，應該很好接。
- 接腳很容易看錯，所以務必小心視差。