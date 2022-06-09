## Final Project

107061125 趙駿騰

___

## Introduction

___


我的期末題目是LU decomposition。藉由LU decomposition可以將一個矩陣  
分解成L下三角矩陣以及U上三角矩陣。在RISCV VP platform執行的版本有  
雙核心控制2PEs與雙核心控制4PEs。因為有設計小數點後6位有效位數，所以得到    
的結果是原本答案的64倍。stratus合成的結果顯示在result部分，會在discussion  
做分析。
___
## Implementation

在執行上，因為題目要求需要我們控制核心來控制資料到各個PE，但因為我的  
題目主要需要執行三個步驟:1.row copy、2.column division、3.submatrix  
update。其中，submatrix update十分適合做PE來進行加速，然而外層的  
column division卻十分不適合，在執行上我將column division分配給  
軟體進行處理，submatrix update用PE來計算。計算結果會送回軟體更新。  
雙核的工作分配我儘量資料傳輸PE的工作評分，他們的工作是將我分配好的  
資料藉由dma送入PE再將結果讀出來。  

stratus合成的部分2PEs與4PEs的版本都有額外合成module pipelining的版本  
1PE的版本主要是讓我們來觀察差異用的。
___
## Result
## RISCV PLATFORM:
two PEs version:   
calculated cycle: 1575
  
four PEs version:   
calculated cycle: 1230

## SYNTHESIZED VERSION:
one PE version:  
simulated time: 17150ns  
one cal 60ns  

two PEs version:  
simulated time: 11550ns   
area:8543

two PEs version with module pipelining:  
simulated time: 14350ns    
area:7473.8

four PEs version:  
simulated time: 9600ns   
area:17086

four PEs version with module pipelining:  
simulated time: 11610ns    
area:14947.6
___
## Discussion
在RISCV VP platform中，他的時間計算並不準確，因此result紀錄的  
cycle是我根據我的硬體與軟體計算的。硬體的計算時間我是從stratus  
的分析中測量出來的，一次計算會花費5cycles。軟體的部份我們要考慮  
輸送資料的byte數以及DMA bandwidth。我的設計是每次傳送的資料是  
4bytes，DMA bandwidth也是4bytes，每次傳輸花費1cycle，在進行  
運算需要兩次寫一次讀，總共需要3bytes。因為我的計算所需5cycles  
在整個估算的cycle中，DMA傳輸佔37.5%。改善的方案可以提高DMA  
bandwidth。  

two PEs與four PEs的cycle照前面所敘述應該會差一半才對，然而  
計算的結果顯示有優化cycle但並沒有想像中多。這是因為當我們使用  
DMA傳送資料時，我們需要等待前面的資料傳完後才能傳下一個。像4個  
PE我就必須等待8個資料傳完才能讓所有PE運算，這是主要的因素造成  
cycle減少的沒想像中多。除此之外，因為每層迴圈所需傳遞的資料數  
並非4的倍數，有時會需要額外1次的傳遞與計算。  

stratus合成的部分可以看到我們在只有一個PE所花的時間明顯比2PEs  
版本還多，但是在模擬4PEs版本時，我發現fifo傳遞也會需要cycle，  
但是就變成我傳輸的時間會是2PEs版本兩倍的量，又因為我的計算單元  
花費時間很短，導致模擬時間的結果與2PEs 版本幾乎一樣。但這是不合  
理的，在實際平行PE計算的電路輸出應該是同一個cycle一起輸出，  
上面的時間結果是我扣除了額外的傳輸時間得到的，實際上還是有加速。  

從上面的面積我們可以看到使用的PE數越多面積也線性增加，使用  
module pipelining的結構因為需要額外的cycle進入其他模組所以  
時間增加但是compiler針對不同module優化使得面積減小。  
綜合上面的分析，以這個project而言，我會覺得2PE的結構比較適合  
這個設計，很大的一個原因是這個project傳輸佔的時間算高，因此增加  
太多PE的加速效果並不會太好，面積反而會線性增加。從2PE的版本雖然  
相比1PE是兩倍大面積，但時間減少32.65%，有不錯的加速。

___
## Github link
https://github.com/jim107061125/EE6470/tree/master/final